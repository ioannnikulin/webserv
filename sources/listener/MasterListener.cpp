#include "MasterListener.hpp"

#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <cerrno>
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
#include "listener/Listener.hpp"
#include "logger/Logger.hpp"

using std::map;
using std::ostringstream;
using std::runtime_error;
using std::set;
using std::string;
using std::vector;

namespace {
using webserver::Connection;
using webserver::Listener;

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
    if (read(pipeFd, &state, sizeof(state)) == -1) {
        throw runtime_error("Failed to read from control pipe");
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
    if (bytesRead == -1) {
        throw runtime_error("Failed to read from response pipe");
    }
    close(pipeFd);
    return (oss.str());
}
}  // namespace

namespace webserver {

Logger MasterListener::_log;

MasterListener::MasterListener(const AppConfig& configuration) {
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

Connection::State MasterListener::generateResponse(Listener* listener, ::pollfd& activeFd) {
    const int READING_PIPE_END = 0;
    const int WRITING_PIPE_END = 1;
    int responsePipe[2];
    int controlPipe[2];
    if (pipe(responsePipe) == -1 || pipe(controlPipe) == -1) {
        throw runtime_error("pipe() failed");
    }
    const int pid = fork();
    if (pid == -1) {
        throw runtime_error("fork() failed");
    }
    if (pid == 0) {
        // NOTE: child; generates response and sends it to pipe for the parent to fetch and dispatch
        if (close(responsePipe[READING_PIPE_END]) == -1) {
            _log.stream(LOG_ERROR) << "close() failed on child's reading response pipe end\n";
        }
        if (close(controlPipe[READING_PIPE_END]) == -1) {
            _log.stream(LOG_ERROR) << "close() failed on child's reading control pipe end\n";
        }
        Connection::State connState = listener->generateResponse(activeFd.fd);
        if (write(controlPipe[WRITING_PIPE_END], &connState, sizeof(connState)) == -1) {
            _log.stream(LOG_ERROR) << "Failed to write to control pipe in child process\n";
        }
        const string response = listener->getResponse(activeFd.fd);
        if (write(responsePipe[WRITING_PIPE_END], response.data(), response.size()) == -1) {
            _log.stream(LOG_ERROR) << "Failed to write to response pipe in child process\n";
        }
        if (close(controlPipe[WRITING_PIPE_END]) == -1) {
            _log.stream(LOG_ERROR) << "close() failed on child's writing control pipe end\n";
        }
        if (close(responsePipe[WRITING_PIPE_END]) == -1) {
            _log.stream(LOG_ERROR) << "close() failed on child's writing response pipe end\n";
        }
        // NOTE: circumventing forbidden _exit()
        char* argv[] = {
            const_cast<char*>("/bin/sh"),
            const_cast<char*>("-c"),
            const_cast<char*>("exit 0"),
            // clang-format off
            NULL
        };
        // clang-format on
        char* envp[] = {NULL};
        execve("/bin/sh", argv, envp);
        // NOTE: should not get to this point, but if it got here, it's a zombie that the parent can kill
        while (true) {
        }
    } else {
        // NOTE: parent; registers the reading end of the pipe in the common poll() queue
        if (close(controlPipe[WRITING_PIPE_END]) == -1 ||
            close(responsePipe[WRITING_PIPE_END]) == -1) {
            throw runtime_error("close() failed on parent's writing pipe ends");
        }
        registerResponseWorker(
            controlPipe[READING_PIPE_END],
            responsePipe[READING_PIPE_END],
            activeFd.fd
        );
    }
    return (Connection::WRITING);
}

void MasterListener::removePollFd(int fdesc) {
    for (vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        if (it->fd == fdesc) {
            _pollFds.erase(it);
            return;
        }
    }
}

Connection::State MasterListener::isItANewConnectionOnAListeningSocket(::pollfd& activeFd) {
    Listener* listener = findListener(_listeners, activeFd.fd);
    if (listener == NULL) {
        return (Connection::IGNORED);
    }
    const int clientSocket = registerNewConnection(activeFd.fd, listener);
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
        return (generateResponse(listener, activeFd));
    }
    return (connState);  // NOTE: READING
}

Connection::State MasterListener::isItAControlMessageFromAResponseGeneratorWorker(::pollfd& activeFd
) {
    const map<int, int>::iterator controlMessageReadyFor =
        _responseWorkerControls.find(activeFd.fd);
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

Connection::State MasterListener::isItAResponseFromAResponseGeneratorWorker(::pollfd& activeFd) {
    const map<int, int>::iterator responseReadyFor = _responseWorkers.find(activeFd.fd);
    if (responseReadyFor == _responseWorkers.end()) {
        return (Connection::IGNORED);
    }
    _log.stream(LOG_TRACE) << "Response for " << responseReadyFor->second
                           << " made by worker on fd " << responseReadyFor->first
                           << " is being picked up by main thread\n";
    const string response = readStringAndClose(responseReadyFor->first);
    _log.stream(LOG_TRACE) << response << "\n";
    _clientListeners.at(responseReadyFor->second)->setResponse(responseReadyFor->second, response);
    markResponseReadyForReturn(responseReadyFor->second);
    removePollFd(responseReadyFor->first);
    _responseWorkers.erase(responseReadyFor);
    return (Connection::WRITING_COMPLETE);
}

Connection::State
MasterListener::handleIncomingConnection(::pollfd& activeFd, bool& acceptingNewConnections) {
    Connection::State ret;
    if (acceptingNewConnections) {
        ret = isItANewConnectionOnAListeningSocket(activeFd);
        if (ret != Connection::IGNORED) {
            return (ret);
        }
    }
    ret = isItADataRequestOnAClientSocketFromARegisteredClient(activeFd);
    if (ret != Connection::IGNORED) {
        return (ret);
    }
    ret = isItAControlMessageFromAResponseGeneratorWorker(activeFd);
    if (ret != Connection::IGNORED) {
        if (ret == Connection::SERVER_SHUTTING_DOWN) {
            acceptingNewConnections = false;
        }
        return (ret);
    }
    ret = isItAResponseFromAResponseGeneratorWorker(activeFd);
    if (ret != Connection::IGNORED) {
        return (ret);
    }
    _log.stream(LOG_WARN) << "Unknown socket fd " << activeFd.fd << " has sent data, ignoring.\n";
    return (Connection::IGNORED);
}

void MasterListener::handleOutgoingConnection(const ::pollfd& activeFd) {
    _log.stream(LOG_TRACE) << "Starting sending response back to " << activeFd.fd << "\n";
    Listener* listener = findListener(_clientListeners, activeFd.fd);

    if (listener == NULL) {
        _log.stream(LOG_WARN) << "Tried to send data to an unknown socket fd " << activeFd.fd
                              << ", ignoring.\n";
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

void MasterListener::listenAndHandle(volatile __sig_atomic_t& isRunning) {
    populateFdsFromListeners();
    bool acceptingNewConnections = true;

    while (isRunning == 1) {
        for (size_t i = 0; i < _pollFds.size(); ++i) {
            _pollFds[i].revents = 0;
        }
        const int ret = poll(_pollFds.data(), _pollFds.size(), -1);
        if (ret == -1) {
            if (errno == EINTR) {
                continue;  // NOTE: interrupted by signal, retry
            }
            throw runtime_error(string("poll() failed: ") + strerror(errno));
        }
        reapChildren();
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

MasterListener::~MasterListener() {
    for (map<int, Listener*>::const_iterator it = _listeners.begin(); it != _listeners.end();
         ++it) {
        delete it->second;
    }  // NOTE: deleting from listeners only, clientListeners contains pointers to the same Listener objects
}
}  // namespace webserver
