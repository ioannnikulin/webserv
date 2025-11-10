#include "MasterListener.hpp"

#include <poll.h>
#include <string.h>

#include <cerrno>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "Listener.hpp"

using std::cerr;
using std::clog;
using std::endl;
using std::map;
using std::pair;
using std::runtime_error;
using std::set;
using std::string;
using std::vector;

namespace {
int registerNewConnection(
    vector<struct ::pollfd>& pollFds,
    int listeningFd,
    webserver::Listener* listener
) {
    clog << "A new connection on socket fd " << listeningFd << endl;
    struct ::pollfd clientPfd;
    clientPfd.fd = listener->acceptConnection();
    clientPfd.events = POLLIN;
    clientPfd.revents = 0;
    pollFds.push_back(clientPfd);
    clog << "Connection accepted, client socket " << clientPfd.fd << " assigned to this client"
         << endl;
    return (clientPfd.fd);
}

using webserver::Listener;

Listener* findListener(map<int, Listener*> where, int byFd) {
    const map<int, Listener*>::const_iterator res = where.find(byFd);
    if (res == where.end()) {
        return NULL;
    }
    return (res->second);
}

void populateFdsFromListeners(
    vector<struct ::pollfd>& pollFds,
    const map<int, Listener*>& listeners
) {
    for (map<int, Listener*>::const_iterator it = listeners.begin(); it != listeners.end(); ++it) {
        struct ::pollfd pfd;
        pfd.fd = it->first;
        pfd.events = POLLIN;
        pfd.revents = 0;
        pollFds.push_back(pfd);
    }
}

}  // namespace

namespace webserver {
MasterListener::MasterListener(const set<pair<string, int> >& interfacePortPairs) {
    for (set<pair<string, int> >::const_iterator it = interfacePortPairs.begin();
         it != interfacePortPairs.end();
         ++it) {
        Listener* newListener = new Listener(it->first, it->second);
        _listeners.at(newListener->getListeningSocketFd()) = newListener;
    }
}

void MasterListener::handleIncomingConnection(vector<struct ::pollfd>& pollFds, int activeFd) {
    Listener* listener = findListener(_listeners, activeFd);
    // is it a request on a LISTENING socket? so a request for a new connection?
    if (listener != NULL) {
        const int clientSocket = registerNewConnection(pollFds, activeFd, listener);
        _clientListeners.at(clientSocket) = listener;
        return;
    }
    // if not, maybe this is a request on an existing connection,
    // so on a CLIENT socket?
    listener = findListener(_clientListeners, activeFd);
    if (listener != NULL) {
        clog << "Existing client on socket fd " << activeFd << " has sent data." << endl;
        listener->receiveRequest(activeFd);
        if (!listener->hasActiveClientSocket(activeFd)) {
            // client disconnected so we remove him from existing sessions list
            _clientListeners.erase(_clientListeners.find(activeFd));
        }
        return;
    }
    // this should never happen in theory, since all sockets come from listening or client lists
    // maybe sending data after a timeout, when the connection was closed already or smth
    cerr << "Unknown socket fd " << activeFd << " has sent data, ignoring." << endl;
}

void MasterListener::listenAndHandle(volatile __sig_atomic_t& isRunning) {
    vector<struct ::pollfd> pollFds;
    populateFdsFromListeners(pollFds, _listeners);

    while (isRunning == 1) {
        const int ret = poll(pollFds.data(), pollFds.size(), -1);  // POLL! the one and only!
        if (ret == -1) {
            if (errno == EINTR) {
                continue;  // interrupted by signal, retry
            }
            throw runtime_error(string("poll() failed: ") + strerror(errno));
        }
        for (size_t i = 0; i < pollFds.size(); ++i) {
            if ((pollFds[i].revents & POLLIN) > 0) {
                // something happened on that listening socket, let's dive in
                handleIncomingConnection(pollFds, pollFds[i].fd);
            }
            if ((pollFds[i].revents & POLLOUT) > 0) {
                // the response is ready to be sent back
                handleOutgoingConnection(pollFds, pollFds[i].fd);
            }
        }
    }
}

MasterListener::~MasterListener() {
    for (map<int, Listener*>::const_iterator it = _listeners.begin(); it != _listeners.end();
         ++it) {
        delete it->second;
    }  // deleting from listeners only, clientListeners contains pointers to the same Listener objects
    // should probably set them to nullptr there though
    // check with valgrind
}
}  // namespace webserver