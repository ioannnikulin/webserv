#include "MasterListener.hpp"

#include <poll.h>
#include <string.h>

// #include <algorithm>
#include <cerrno>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "../utils/colors.hpp"
#include "../utils/utils.hpp"
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
    clog << B_YELLOW << "A new connection on socket fd " << listeningFd << endl << RESET;
    struct ::pollfd clientPfd;
    clientPfd.fd = listener->acceptConnection();
    clientPfd.events = POLLIN;
    clientPfd.revents = 0;
    pollFds.push_back(clientPfd);
    listener->setClientSocket(&pollFds.back());
    clog << "Connection accepted, client socket " << clientPfd.fd << " assigned to this client."
         << endl;
    return (clientPfd.fd);
}

using webserver::Listener;

Listener* findListener(map<int, Listener*> where, int byFd) {
    const map<int, Listener*>::const_iterator res = where.find(byFd);
    if (res == where.end()) {
        return (NULL);
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
        _listeners[newListener->getListeningSocketFd()] = newListener;
    }
}

void MasterListener::handleIncomingConnection(::pollfd& activeFd) {
    Listener* listener = findListener(_listeners, activeFd.fd);
    // NOTE: is it a request on a LISTENING socket? so a request for a new connection?
    if (listener != NULL) {
        const int clientSocket = registerNewConnection(_pollFds, activeFd.fd, listener);
        _clientListeners[clientSocket] = listener;
        return;
    }
    // NOTE: if not, maybe this is a request on an existing connection,
    // NOTE: so on a CLIENT socket?
    listener = findListener(_clientListeners, activeFd.fd);
    if (listener != NULL) {
        clog << "Existing client on socket fd " << activeFd.fd << " has sent data." << endl;
        listener->receiveRequest(activeFd);
        if (!listener->hasActiveClientSocket(activeFd.fd)) {
            // NOTE: client disconnected so we remove him from existing sessions list
            _clientListeners.erase(_clientListeners.find(activeFd.fd));
        }
        return;
    }
    // NOTE: this should never happen in theory, since all sockets come from listening or client lists
    // NOTE: maybe sending data after a timeout, when the connection was closed already or smth
    cerr << "Unknown socket fd " << activeFd.fd << " has sent data, ignoring." << endl;
}

void MasterListener::handleOutgoingConnection(const ::pollfd& activeFd) const {
    Listener* listener = findListener(_clientListeners, activeFd.fd);

    if (listener == NULL) {
        cerr << B_RED << "Tried to send data to an unknown socket fd " << activeFd.fd
             << ", ignoring." << endl
             << RESET;
        return;
    }
    listener->sendResponse(activeFd.fd);
    // NOTE: no keep-alive, so killing right away
    // NOTE: if he wants to go on, he'd have to go to listening socket again
    listener->killConnection(activeFd.fd);
    clog << "✅ " << B_GREEN << "Sent response to socket fd " << activeFd.fd << endl << RESET;
    output_formatting::printSeparator();
}

void MasterListener::listenAndHandle(volatile __sig_atomic_t& isRunning) {
    populateFdsFromListeners(_pollFds, _listeners);

    while (isRunning == 1) {
        const int ret =
            poll(_pollFds.data(), _pollFds.size(), -1);  // NOTE: POLL! the one and only!
        if (ret == -1) {
            if (errno == EINTR) {
                continue;  // NOTE: interrupted by signal, retry
            }
            throw runtime_error(string("poll() failed: ") + strerror(errno));
        }
        for (size_t i = 0; i < _pollFds.size();) {
            if ((_pollFds[i].revents & POLLIN) > 0) {
                // NOTE: something happened on that listening socket, let's dive in
                handleIncomingConnection(_pollFds[i]);
                i++;
                continue;
            }
            if ((_pollFds[i].revents & POLLOUT) > 0) {
                // NOTE: the response is ready to be sent back
                handleOutgoingConnection(_pollFds[i]);
                _pollFds.erase(
                    _pollFds.begin() + static_cast<std::vector<struct ::pollfd>::difference_type>(i)
                );
                continue;
            }
            i++;
        }
    }
}

MasterListener::~MasterListener() {
    for (map<int, Listener*>::const_iterator it = _listeners.begin(); it != _listeners.end();
         ++it) {
        delete it->second;
    }  // NOTE: deleting from listeners only, clientListeners contains pointers to the same Listener objects
    // TODO: should probably set them to nullptr there though
    // TODO: check with valgrind
}
}  // namespace webserver
