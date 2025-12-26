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

#include <connection/Connection.hpp>
#include "configuration/AppConfig.hpp"
#include "http_status/ShuttingDown.hpp"
#include "listener/Listener.hpp"
#include "utils/colors.hpp"
#include "utils/utils.hpp"

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
using webserver::Listener;

int registerNewConnection(
    vector<struct ::pollfd>& pollFds,
    int listeningFd,
    Listener* listener
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
MasterListener::MasterListener(const AppConfig& configuration) {
    const set<Endpoint>& endpoints = configuration.getEndpoints();
    for (set<Endpoint>::const_iterator itr = endpoints.begin();
         itr != endpoints.end();
         ++itr) {
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

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void MasterListener::handleIncomingConnection(
    ::pollfd& activeFd,
    bool shouldDeny
) {
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
        bool requestTermination = false;
        try {
            listener->receiveRequest(activeFd, shouldDeny);
            clog << "Request received" << endl;
        } catch (const ShuttingDown& e) {
            requestTermination = true;
        } catch (const Connection::TerminatedByClient& e) {
            clog << "Client disconnected, terminating" << endl;
            listener->killConnection(activeFd.fd);
            _clientListeners.erase(_clientListeners.find(activeFd.fd));
        }
        if (requestTermination) {
            throw ShuttingDown();
        }
        return;
    }
    // NOTE: this should never happen in theory, since all sockets come from listening or client lists
    // NOTE: maybe sending data after a timeout, when the connection was closed already or smth
    cerr << "Unknown socket fd " << activeFd.fd << " has sent data, ignoring." << endl;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void MasterListener::handleOutgoingConnection(const ::pollfd& activeFd) {
    clog << "Starting sending response back to " << activeFd.fd << endl;
    Listener* listener = findListener(_clientListeners, activeFd.fd);

    if (listener == NULL) {
        cerr << B_RED << "Tried to send data to an unknown socket fd " << activeFd.fd
             << ", ignoring." << endl
             << RESET;
        return;
    }
    listener->sendResponse(activeFd.fd);
    // NOTE: no keep-alive in HTTP 1.0, so killing right away
    // NOTE: if he wants to go on, he'd have to go to listening socket again
    utils::printSeparator();
    utils::setColor(B_GREEN);
    std::cout << "📨 Sent response to socket fd " << activeFd.fd << endl;
    utils::resetColor();
    utils::printSeparator();
    _clientListeners.erase(_clientListeners.find(activeFd.fd));
    listener->killConnection(activeFd.fd);
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static)
void MasterListener::listenAndHandle(
    volatile __sig_atomic_t& isRunning
) {
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
                try {
                    handleIncomingConnection(_pollFds[i], isRunning == 0);
                } catch (const ShuttingDown& e) {
                    isRunning = 0;
                }
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
    // NOTE: should probably set them to nullptr there though
    // TODO 56: check with valgrind
}
}  // namespace webserver
