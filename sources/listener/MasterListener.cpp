#include "MasterListener.hpp"

#include <signal.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <cerrno>
#include <exception>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "configuration/AppConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "connection/Connection.hpp"
#include "file_system/FileSystem.hpp"
#include "http_status/HttpStatus.hpp"
#include "listener/Listener.hpp"
#include "logger/Logger.hpp"
#include "response/Response.hpp"
#include "signals/ServerSignal.hpp"

using std::map;
using std::ostringstream;
using std::runtime_error;
using std::set;
using std::string;
using std::vector;

namespace {
using webserver::Connection;
using webserver::Listener;
using webserver::Logger;

Listener* findListener(map<int, Listener*> where, int byFd) {
    const map<int, Listener*>::const_iterator res = where.find(byFd);
    if (res == where.end()) {
        return (NULL);
    }
    return (res->second);
}

void markConnectionClosedToAvoidRequestOverlapping(::pollfd& activeFd) {
    activeFd.events = 0;
}

Connection::State readControlMessageAndClose(int pipeFd) {
    Connection::State state;
    const ssize_t result = read(pipeFd, &state, sizeof(state));

    if (result == -1) {
        close(pipeFd);
        throw runtime_error("Failed to read from control pipe");
    }

    if (result != sizeof(state)) {
        close(pipeFd);
        throw runtime_error("Incomplete read from control pipe");
    }

    close(pipeFd);
    return (state);
}

string readStringAndClose(int pipeFd) {
    const int RESPONSE_STRING_BUFFER_SIZE = 4096;
    char buffer[RESPONSE_STRING_BUFFER_SIZE];
    ostringstream oss;
    ssize_t bytesRead = 0;

    while ((bytesRead = read(pipeFd, buffer, sizeof(buffer))) > 0) {
        oss << string(buffer, bytesRead);
    }

    close(pipeFd);
    return (oss.str());
}

}  // namespace

namespace webserver {

Logger MasterListener::_log;

MasterListener::MasterListener(const AppConfig& configuration)
    : _cgiManager(_log) {
    const set<Endpoint>& endpoints = configuration.getEndpoints();
    for (set<Endpoint>::const_iterator itr = endpoints.begin(); itr != endpoints.end(); ++itr) {
        Listener* newListener = new Listener(*itr);
        _listeners[newListener->getListeningSocketFd()] = newListener;
    }
}

MasterListener& MasterListener::operator=(const MasterListener& other) {
    if (this == &other) {
        return (*this);
    }
    _pollFds = other._pollFds;
    _listeners = other._listeners;
    _clientListeners = other._clientListeners;
    return (*this);
}

int MasterListener::registerNewConnection(int listeningFd, Listener* listener) {
    _log.stream(LOG_DEBUG) << "A new connection on socket fd " << listeningFd << "\n";
    struct ::pollfd clientPfd;
    clientPfd.fd = listener->acceptConnection();
    clientPfd.events = POLLIN;
    clientPfd.revents = 0;
    _pollFds.push_back(clientPfd);
    _log.stream(LOG_DEBUG) << "Connection accepted, client socket " << clientPfd.fd << "\n";
    return (clientPfd.fd);
}

void MasterListener::populateFdsFromListeners() {
    for (map<int, Listener*>::const_iterator it = _listeners.begin(); it != _listeners.end();
         ++it) {
        struct ::pollfd pfd;
        pfd.fd = it->first;
        pfd.events = POLLIN;
        pfd.revents = 0;
        _pollFds.push_back(pfd);
    }
}

void MasterListener::registerResponseWorker(
    int controlPipeReadingEnd,
    int responsePipeReadingEnd,
    int clientFd
) {
    struct ::pollfd controlPipePollFd;
    controlPipePollFd.fd = controlPipeReadingEnd;
    controlPipePollFd.events = POLLIN;
    controlPipePollFd.revents = 0;
    _pollFds.push_back(controlPipePollFd);
    struct ::pollfd responsePipePollFd;
    responsePipePollFd.fd = responsePipeReadingEnd;
    responsePipePollFd.events = POLLIN;
    responsePipePollFd.revents = 0;
    _pollFds.push_back(responsePipePollFd);
    _responseWorkerControls[controlPipeReadingEnd] = clientFd;
    _responseWorkers[responsePipeReadingEnd] = clientFd;
}

void MasterListener::markResponseReadyForReturn(int clientFd) {
    for (vector<struct ::pollfd>::iterator itr = _pollFds.begin(); itr != _pollFds.end(); itr++) {
        if (itr->fd == clientFd) {
            itr->events = POLLOUT;
            return;
        }
    }
}

Connection::State MasterListener::callCgi(Listener* listener, int activeFd) {
    const string requestBody = listener->getRequestBody(activeFd);

    int controlPipeReadEnd = -1;
    int responsePipeReadEnd = -1;

    const pid_t pid = _cgiManager.startCgiProcess(
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
        connState != Connection::SERVER_SHUTTING_DOWN) {
        _log.stream(LOG_WARN) << "Connection in unexpected state " << connState << "\n";
    }
    markResponseReadyForReturn(activeFd);
    return (connState);
}

void MasterListener::removePollFd(int fdesc) {
    for (vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        if (it->fd == fdesc) {
            _pollFds.erase(it);
            return;
        }
    }
}

Connection::State MasterListener::isItANewConnectionOnAListeningSocket(int activeFd) {
    Listener* listener = findListener(_listeners, activeFd);
    if (listener == NULL) {
        return (Connection::IGNORED);
    }
    const int clientSocket = registerNewConnection(activeFd, listener);
    _clientListeners[clientSocket] = listener;
    return (Connection::NEWBORN);
}

Connection::State MasterListener::isItADataRequestOnAClientSocketFromARegisteredClient(
    ::pollfd& activeFd
) {
    Listener* listener = findListener(_clientListeners, activeFd.fd);
    if (listener == NULL) {
        return (Connection::IGNORED);
    }
    _log.stream(LOG_TRACE) << "Existing client on socket fd " << activeFd.fd << " has sent data\n";
    const Connection::State connState = listener->receiveRequest(activeFd.fd);
    if (connState == Connection::CLOSED_BY_CLIENT) {
        _log.stream(LOG_TRACE) << "Client on socket fd " << activeFd.fd
                               << " closed the connection before completing the request\n";
        markConnectionClosedToAvoidRequestOverlapping(activeFd);
        _clientListeners.erase(_clientListeners.find(activeFd.fd));
        listener->killConnection(activeFd.fd);
        removePollFd(activeFd.fd);
        return (connState);
    }
    if (connState == Connection::READING_COMPLETE) {
        markConnectionClosedToAvoidRequestOverlapping(activeFd);
        return (generateResponse(listener, activeFd.fd));
    }
    if (connState == Connection::READING_CGI_REQUEST_COMPLETE) {
        markConnectionClosedToAvoidRequestOverlapping(activeFd);
        return (callCgi(listener, activeFd.fd));
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

    _log.stream(LOG_TRACE) << "Response for " << responseReadyFor->second
                           << " made by worker on fd " << responseReadyFor->first
                           << " is being picked up by main thread\n";
    const string rawOutput = readStringAndClose(responseReadyFor->first);
    _log.stream(LOG_TRACE) << rawOutput << "\n";

    string finalResponse;
    if (_cgiManager.isWorker(clientFd)) {
        _log.stream(LOG_DEBUG) << "Parsing CGI output for client " << clientFd << "\n";
        finalResponse = CgiProcessManager::parseCgiResponse(rawOutput);
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
            acceptingNewConnections = false;
        }
        return (ret);
    }
    ret = isItAControlMessageFromAResponseGeneratorWorker(activeFd.fd);
    if (ret != Connection::IGNORED) {
        if (ret == Connection::SERVER_SHUTTING_DOWN) {
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
    _clientListeners.erase(_clientListeners.find(activeFd.fd));
    listener->killConnection(activeFd.fd);
    removePollFd(activeFd.fd);
}

void MasterListener::resetPollEvents() {
    for (size_t i = 0; i < _pollFds.size(); ++i) {
        _pollFds[i].revents = 0;
    }
}

void MasterListener::handlePollEvents(bool& acceptingNewConnections) {
    for (size_t i = 0; i < _pollFds.size(); i++) {
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
        if ((_pollFds[i].revents & (POLLERR | POLLHUP | POLLNVAL)) > 0) {
            close(_pollFds[i].fd);
            removePollFd(_pollFds[i].fd);
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
            if (errno == EINTR) {
                if (((signals & SIG_SHUTDOWN) != 0)) {
                    _log.stream(LOG_INFO)
                        << "Shutdown requested; stopped accepting new connections\n";
                    acceptingNewConnections = false;

                    const vector<int> timedOut = _cgiManager.checkTimeouts();
                    for (size_t i = 0; i < timedOut.size(); ++i) {
                        const pid_t pid = _cgiManager.getProcessId(timedOut[i]);
                        if (pid > 0) {
                            kill(pid, SIGKILL);
                        }
                        cleanupCgiProcess(timedOut[i], false);
                    }

                    isRunning = 0;
                }
                continue;
            }
            throw runtime_error(string("poll() failed: ") + strerror(errno));
        }
        checkCgiTimeouts();
        reapChildren();
        handlePollEvents(acceptingNewConnections);
        if (!acceptingNewConnections && _clientListeners.empty()) {
            isRunning = 0;
        }
    }
}

void MasterListener::reapChildren() {
    int status = 0;
    pid_t pid;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            _log.stream(LOG_DEBUG)
                << "Child " << pid << " exited with code " << WEXITSTATUS(status) << "\n";
        } else if (WIFSIGNALED(status)) {
            _log.stream(LOG_DEBUG)
                << "Child " << pid << " killed by signal " << WTERMSIG(status) << "\n";
        }
    }
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

            const string pageLocation =
                HttpStatus::getPageFileLocation(HttpStatus::GATEWAY_TIMEOUT);
            string errorPageContent;

            try {
                errorPageContent = file_system::readFile(pageLocation.c_str());
            } catch (const std::exception& e) {
                errorPageContent = "CGI script execution timed out";
            }

            Response timeoutResponse(HttpStatus::GATEWAY_TIMEOUT, errorPageContent, "text/html");
            timeoutResponse.setHeader("Connection", "close");
            listener->setResponse(clientFd, timeoutResponse.serialize());
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

MasterListener::~MasterListener() {
    for (map<int, Listener*>::iterator it = _clientListeners.begin(); it != _clientListeners.end();
         ++it) {
        it->second->killConnection(it->first);
    }
    for (map<int, Listener*>::const_iterator it = _listeners.begin(); it != _listeners.end();
         ++it) {
        delete it->second;
    }  // NOTE: deleting from listeners only, clientListeners contains pointers to the same Listener objects
}

}  // namespace webserver
