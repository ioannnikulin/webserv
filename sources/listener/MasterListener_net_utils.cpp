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

namespace webserver {
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

void MasterListener::removePollFd(int fdesc) {
    for (vector<pollfd>::iterator it = _pollFds.begin(); it != _pollFds.end(); ++it) {
        if (it->fd == fdesc) {
            _pollFds.erase(it);
            return;
        }
    }
}

void MasterListener::resetPollEvents() {
    for (size_t i = 0; i < _pollFds.size(); ++i) {
        _pollFds[i].revents = 0;
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

}  // namespace webserver
