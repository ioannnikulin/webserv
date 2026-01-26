#include "MasterListener.hpp"

#include <signal.h>
#include <string.h>
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>

#include <cerrno>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cgi_handler/CgiProcessManager.hpp"
#include "connection/Connection.hpp"
#include "http_status/HttpStatus.hpp"
#include "listener/Listener.hpp"
#include "logger/Logger.hpp"
#include "request/Request.hpp"
#include "response/Response.hpp"
#include "signals/ServerSignal.hpp"

using std::map;
using std::ostringstream;
using std::runtime_error;
using std::string;
using std::vector;

namespace webserver {
Connection::State MasterListener::callCgi(Listener* listener, int activeFd) {
    _log.stream(LOG_TRACE) << "processing cgi request: " << listener->getRequestFor(activeFd)
                           << "\n";
    const string requestBody = listener->getRequestBody(activeFd);

    int controlPipeReadEnd = -1;
    int responsePipeReadEnd = -1;

    const pid_t pid = CgiProcessManager::startCgiProcess(
        listener,
        activeFd,
        requestBody,
        controlPipeReadEnd,
        responsePipeReadEnd
    );

    _cgiManager.registerWorker(activeFd, pid);
    registerResponseWorker(controlPipeReadEnd, responsePipeReadEnd, activeFd);
    return (Connection::WRITING);
}

Connection::State MasterListener::generateResponse(Listener* listener, int activeFd) {
    const Connection::State connState = listener->generateResponse(activeFd);
    if (connState != Connection::WRITING_COMPLETE &&
        connState != Connection::SERVER_SHUTTING_DOWN &&
        connState != Connection::REROUTING_BACK_TO_CGI) {
        _log.stream(LOG_WARN) << "Connection in unexpected state " << connState << "\n";
    }
    if (connState != Connection::REROUTING_BACK_TO_CGI) {
        markResponseReadyForReturn(activeFd);
    }
    return (connState);
}

Connection::State MasterListener::isItANewConnectionOnAListeningSocket(int activeFd) {
    Listener* listener = findListener(_listeners, activeFd);
    if (listener == NULL) {
        return (Connection::IGNORED);
    }
    const int clientSocket = registerNewConnection(activeFd, listener);
    _clientListeners[clientSocket] = listener;
    _log.stream(LOG_TRACE) << "CONN_TRACK: Added fd " << clientSocket
                           << " to _clientListeners map (total: " << _clientListeners.size()
                           << ")\n";
    return (Connection::NEWBORN);
}

Connection::State MasterListener::isItADataRequestOnAClientSocketFromARegisteredClient(
    ::pollfd& activeFd
) {
    Listener* listener = findListener(_clientListeners, activeFd.fd);
    if (listener == NULL) {
        return (Connection::IGNORED);
    }
    _log.stream(LOG_TRACE) << "Existing client on socket fd " << activeFd.fd << " has sent data\n"
                           << "CONN_TRACK: Processing data for fd " << activeFd.fd << "\n";
    Connection::State connState = listener->receiveRequest(activeFd.fd);
    if (connState == Connection::CLOSED_BY_CLIENT) {
        _log.stream(LOG_TRACE) << "Client on socket fd " << activeFd.fd
                               << " closed the connection before completing the request\n";
        markConnectionClosedToAvoidRequestOverlapping(activeFd);
        _clientListeners.erase(_clientListeners.find(activeFd.fd));
        listener->killConnection(activeFd.fd);
        removePollFd(activeFd.fd);
        return (connState);
    }
    if (connState == Connection::READING_COMPLETE || connState == Connection::METHOD_NOT_ALLOWED ||
        connState == Connection::BAD_REQUEST_READ) {
        markConnectionClosedToAvoidRequestOverlapping(activeFd);
        connState = generateResponse(listener, activeFd.fd);
        if (connState == Connection::REROUTING_BACK_TO_CGI) {
            return (callCgi(listener, activeFd.fd));
        }
    }
    return (connState);  // NOTE: READING
}

Connection::State MasterListener::isItAControlMessageFromAResponseGeneratorWorker(int activeFd) {
    const map<int, int>::iterator controlMessageReadyFor = _responseWorkerControls.find(activeFd);
    if (controlMessageReadyFor == _responseWorkerControls.end()) {
        return (Connection::IGNORED);
    }
    const Connection::State connState = readControlMessageAndClose(controlMessageReadyFor->first);
    _log.stream(LOG_TRACE) << "Worker on fd " << controlMessageReadyFor->first
                           << " reported status " << connState << "\n";
    removePollFd(controlMessageReadyFor->first);
    _responseWorkerControls.erase(controlMessageReadyFor);
    return (connState);
}

Connection::State MasterListener::isItAResponseFromAResponseGeneratorWorker(int activeFd) {
    const map<int, int>::iterator responseReadyFor = _responseWorkers.find(activeFd);
    if (responseReadyFor == _responseWorkers.end()) {
        return (Connection::IGNORED);
    }

    const int clientFd = responseReadyFor->second;

    _log.stream(LOG_TRACE) << "Response for " << clientFd << " made by worker on fd "
                           << responseReadyFor->first << " is being picked up by main thread\n";
    const string rawOutput = readStringAndClose(responseReadyFor->first);
    _log.stream(LOG_TRACE) << rawOutput << "\n";

    string finalResponse;
    if (_cgiManager.isWorker(clientFd)) {
        _log.stream(LOG_DEBUG) << "Parsing CGI output for client " << clientFd << "\n";
        Listener* client = findListener(_clientListeners, clientFd);
        if (client == NULL) {
            _log.stream(LOG_ERROR) << "Couldn't find client listener for client " << clientFd
                                   << ", cannot validate original request, cannot form a response, "
                                      "aborting connection\n";
            finalResponse = HttpStatus::ultimateInternalServerError().serialize();
        } else {
            finalResponse =
                CgiProcessManager::parseCgiResponse(rawOutput, client->getConfiguration());
        }
        _cgiManager.unregisterWorker(clientFd);
    } else {
        finalResponse = rawOutput;
    }

    _clientListeners.at(clientFd)->setResponse(clientFd, finalResponse);
    markResponseReadyForReturn(clientFd);
    removePollFd(responseReadyFor->first);
    _responseWorkers.erase(responseReadyFor);
    return (Connection::WRITING_COMPLETE);
}

Connection::State
MasterListener::handleIncomingConnection(::pollfd& activeFd, bool& acceptingNewConnections) {
    Connection::State ret;
    if (acceptingNewConnections) {
        ret = isItANewConnectionOnAListeningSocket(activeFd.fd);
        if (ret != Connection::IGNORED) {
            return (ret);
        }
    }
    ret = isItADataRequestOnAClientSocketFromARegisteredClient(activeFd);
    if (ret != Connection::IGNORED) {
        if (ret == Connection::SERVER_SHUTTING_DOWN) {
            handleShutdownSignal();
            acceptingNewConnections = false;
        }
        return (ret);
    }
    ret = isItAControlMessageFromAResponseGeneratorWorker(activeFd.fd);
    if (ret != Connection::IGNORED) {
        if (ret == Connection::SERVER_SHUTTING_DOWN) {
            handleShutdownSignal();
            acceptingNewConnections = false;
        }
        return (ret);
    }
    ret = isItAResponseFromAResponseGeneratorWorker(activeFd.fd);
    if (ret != Connection::IGNORED) {
        return (ret);
    }
    _log.stream(LOG_WARN) << "Unknown socket fd " << activeFd.fd << " has sent data, ignoring\n";
    return (Connection::IGNORED);
}

void MasterListener::handleOutgoingConnection(const ::pollfd& activeFd) {
    _log.stream(LOG_TRACE) << "Starting sending response back to " << activeFd.fd << "\n";
    Listener* listener = findListener(_clientListeners, activeFd.fd);

    if (listener == NULL) {
        _log.stream(LOG_WARN) << "Tried to send data to an unknown socket fd " << activeFd.fd
                              << ", ignoring\n";
        return;
    }
    listener->sendResponse(activeFd.fd);
    // NOTE: no keep-alive in HTTP 1.0, so killing right away
    // NOTE: if he wants to go on, he'd have to go to listening socket again
    _log.stream(LOG_INFO) << "Sent response to socket fd " << activeFd.fd << "\n";
    _log.stream(LOG_TRACE) << "CONN_TRACK: Removing fd " << activeFd.fd
                           << " from _clientListeners (before: " << _clientListeners.size()
                           << ")\n";
    _clientListeners.erase(_clientListeners.find(activeFd.fd));
    _log.stream(LOG_TRACE) << "CONN_TRACK: Removed fd " << activeFd.fd
                           << " from _clientListeners (after: " << _clientListeners.size() << ")\n";
    listener->killConnection(activeFd.fd);
    removePollFd(activeFd.fd);
}

Connection::State MasterListener::handleResponseWorkerContent(int activeFd) {
    const map<int, int>::iterator responseIt = _responseWorkers.find(activeFd);
    if (responseIt == _responseWorkers.end()) {
        return (Connection::IGNORED);
    }
    const int clientFd = responseIt->second;
    _log.stream(LOG_DEBUG) << "Response pipe " << activeFd << " closed for client " << clientFd
                           << "\n";

    const string rawOutput = readStringAndClose(activeFd);

    string finalResponse;
    if (_cgiManager.isWorker(clientFd)) {
        _log.stream(LOG_DEBUG) << "Parsing CGI output for client " << clientFd << "\n";
        Listener* client = findListener(_clientListeners, clientFd);
        if (client == NULL) {
            _log.stream(LOG_ERROR) << "Couldn't find client listener for client " << clientFd
                                   << ", cannot validate original request, cannot form a response, "
                                      "aborting connection\n";
            finalResponse = HttpStatus::ultimateInternalServerError().serialize();
            markResponseReadyForReturn(clientFd);
            removePollFd(activeFd);
            _responseWorkers.erase(responseIt);
        }
        finalResponse = CgiProcessManager::parseCgiResponse(rawOutput, client->getConfiguration());
        _cgiManager.unregisterWorker(clientFd);
    } else {
        finalResponse = rawOutput;
    }

    _clientListeners.at(clientFd)->setResponse(clientFd, finalResponse);
    markResponseReadyForReturn(clientFd);
    removePollFd(activeFd);
    _responseWorkers.erase(responseIt);
    return (Connection::RECEIVED_RESPONSE_FROM_WORKER);
}

Connection::State MasterListener::handleResponseWorkerStatusReport(int activeFd) {
    const map<int, int>::iterator controlIt = _responseWorkerControls.find(activeFd);
    if (controlIt == _responseWorkerControls.end()) {
        return (Connection::IGNORED);
    }
    _log.stream(LOG_DEBUG) << "Control pipe " << activeFd
                           << " closed (peer exited or closed early)\n";
    close(activeFd);
    removePollFd(activeFd);
    _responseWorkerControls.erase(controlIt);
    return (Connection::RECEIVED_STATUS_FROM_WORKER);
}

void MasterListener::handlePollEvents(bool& acceptingNewConnections) {
    for (size_t i = 0; i < _pollFds.size(); i++) {
        if ((_pollFds[i].revents & (POLLHUP | POLLERR)) > 0) {
            if (handleResponseWorkerContent(_pollFds[i].fd) ==
                Connection::RECEIVED_RESPONSE_FROM_WORKER) {
                continue;
            }
            if (handleResponseWorkerStatusReport(_pollFds[i].fd) ==
                Connection::RECEIVED_STATUS_FROM_WORKER) {
                continue;
            }
            close(_pollFds[i].fd);
            removePollFd(_pollFds[i].fd);
            continue;
        }

        if ((_pollFds[i].revents & POLLIN) > 0) {
            // NOTE: something happened on that listening socket, let's dive in
            handleIncomingConnection(_pollFds[i], acceptingNewConnections);
            continue;
        }
        if ((_pollFds[i].revents & POLLOUT) > 0) {
            // NOTE: the response is ready to be sent back
            handleOutgoingConnection(_pollFds[i]);
            continue;
        }
    }
}

void MasterListener::listenAndHandle(
    volatile __sig_atomic_t& isRunning,
    volatile __sig_atomic_t& signals
) {
    bool acceptingNewConnections = true;

    populateFdsFromListeners();
    while (isRunning == 1) {
        resetPollEvents();
        const int ret = poll(_pollFds.data(), _pollFds.size(), -1);
        if (ret == -1) {
            if (errno != EINTR) {
                throw runtime_error(string("poll() failed: ") + strerror(errno));
            }
            if ((signals & SIG_SHUTDOWN) != 0) {
                handleShutdownSignal();
                acceptingNewConnections = false;
            }
        }
        checkCgiTimeouts();
        reapChildren();
        handlePollEvents(acceptingNewConnections);
        if (!acceptingNewConnections && shouldContinueRunning()) {
            isRunning = 0;
        }
    }
}

void MasterListener::handleShutdownSignal() {
    _log.stream(LOG_INFO) << "Shutdown requested; stopped accepting new connections; "
                          << _clientListeners.size() << " existing connections left\n";

    cleanupTimedOutCgiProcesses();
    cleanupIdleConnections();
}

void MasterListener::cleanupTimedOutCgiProcesses() {
    const vector<int> timedOut = _cgiManager.checkTimeouts();
    for (size_t i = 0; i < timedOut.size(); ++i) {
        const pid_t pid = _cgiManager.getProcessId(timedOut[i]);
        if (pid > 0) {
            kill(pid, SIGKILL);
        }
        cleanupCgiProcess(timedOut[i], false);
    }
}

void MasterListener::cleanupIdleConnections() {
    for (map<int, Listener*>::iterator it = _clientListeners.begin();
         it != _clientListeners.end();) {
        const int clientFd = it->first;
        Listener* listener = it->second;

        if (!listener->hasActiveClientSocket(clientFd)) {
            _log.stream(LOG_WARN) << "Connection fd " << clientFd
                                  << " in _clientListeners but not in listener's connections!\n";
            _clientListeners.erase(it);
            it = _clientListeners.begin();
            continue;
        }

        const Request req = listener->getRequestFor(clientFd);
        if (!req.isRequestTargetReceived()) {
            _log.stream(LOG_INFO) << "Forcing closure of idle connection fd " << clientFd
                                  << " during shutdown\n";
            removePollFd(clientFd);
            listener->killConnection(clientFd);
            _clientListeners.erase(it);
            it = _clientListeners.begin();
            continue;
        }

        ++it;
    }
}

bool MasterListener::shouldContinueRunning() const {
    _log.stream(LOG_TRACE) << "CONN_TRACK: Dumping all _clientListeners entries:\n";
    for (map<int, Listener*>::const_iterator itr = _clientListeners.begin();
         itr != _clientListeners.end();
         itr++) {
        _log.stream(LOG_TRACE) << "  CONN_TRACK: fd=" << itr->first << " listener=" << itr->second
                               << " hasActiveClientSocket="
                               << itr->second->hasActiveClientSocket(itr->first) << "\n";
        try {
            _log.stream(LOG_TRACE)
                << "  CONN_TRACK: request=" << itr->second->getRequestFor(itr->first) << "\n";
        } catch (...) {
            _log.stream(LOG_TRACE) << "  CONN_TRACK: (no request available)\n";
        }
    }
    return (_clientListeners.empty());
}

void MasterListener::cleanupCgiProcess(int clientFd, bool sendTimeoutResponse) {
    _log.stream(LOG_DEBUG) << "Cleaning up CGI process for client " << clientFd << "\n";

    for (map<int, int>::iterator it = _responseWorkers.begin(); it != _responseWorkers.end();) {
        if (it->second == clientFd) {
            close(it->first);
            removePollFd(it->first);
            const map<int, int>::iterator toErase = it;
            ++it;
            _responseWorkers.erase(toErase);
        } else {
            ++it;
        }
    }

    for (map<int, int>::iterator it = _responseWorkerControls.begin();
         it != _responseWorkerControls.end();) {
        if (it->second == clientFd) {
            close(it->first);
            removePollFd(it->first);
            const map<int, int>::iterator toErase = it;
            ++it;
            _responseWorkerControls.erase(toErase);
        } else {
            ++it;
        }
    }

    _cgiManager.cleanupProcess(clientFd);

    if (sendTimeoutResponse) {
        const map<int, Listener*>::iterator listenerIt = _clientListeners.find(clientFd);
        if (listenerIt != _clientListeners.end()) {
            Listener* listener = listenerIt->second;

            listener->setResponse(
                clientFd,
                listenerIt->second->getConfiguration()
                    .getStatusCatalogue()
                    .serveStatusPage(HttpStatus::GATEWAY_TIMEOUT)
                    .serialize()
            );
            markResponseReadyForReturn(clientFd);
        }
    }
}

void MasterListener::checkCgiTimeouts() {
    const vector<int> timedOutFds = _cgiManager.checkTimeouts();

    for (size_t i = 0; i < timedOutFds.size(); ++i) {
        cleanupCgiProcess(timedOutFds[i], true);
    }
}

}  // namespace webserver
