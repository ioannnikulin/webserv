#include "Listener.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

using std::cerr;
using std::clog;
using std::endl;
using std::runtime_error;
using std::strerror;
using std::string;

namespace {
int setupSocket() {
    const int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd == -1) {
        throw runtime_error(string("socket() failed: ") + strerror(errno));
    }
    int opt = 1;
    if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        close(socketFd);
        throw runtime_error(string("setsockopt() failed: ") + strerror(errno));
    }
    const int flags = fcntl(socketFd, F_GETFL, 0);  // not sure, subject forbids this flag for MacOS
    if (flags == -1) {
        throw runtime_error(string("fcntl(F_GETFL) failed: ") + strerror(errno));
    }
    if (fcntl(socketFd, F_SETFL, flags | O_NONBLOCK) == -1) {
        throw runtime_error(string("fcntl(F_SETFL) failed: ") + strerror(errno));
    }
    return (socketFd);
}
}  // namespace

namespace webserver {
Listener& Listener::operator=(const Listener& other) {
    cerr << "Unexpected stub assignment operator called for Listener." << endl;
    _port = other._port;
    _interface = other._interface;
    _listeningSocketFd = 0;
    return (*this);
}

Listener::Listener(const std::string& interface, int port)
    : _interface(interface)
    , _port(port)
    , _listeningSocketFd(-1) {
    _listeningSocketFd = setupSocket();

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    // probably this is wrong - we're taking the socket before the server actually starts up,
    // and report about listening on this socket before calling start().
    // should separate preparation from actual binding to sockets
    if (bind(_listeningSocketFd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1) {
        close(_listeningSocketFd);
        throw runtime_error(string("bind() failed: ") + strerror(errno));
    }

    if (listen(_listeningSocketFd, SOMAXCONN) == -1) {
        close(_listeningSocketFd);
        throw runtime_error(string("listen() failed: ") + strerror(errno));
    }

    clog << "Listening on " << _interface << ":" << _port << " via socket " << _listeningSocketFd
         << endl;
}

int Listener::getListeningSocketFd() const {
    return _listeningSocketFd;
}

bool Listener::hasActiveClientSocket(int clientSocketFd) const {
    return (_clientConnections.find(clientSocketFd) != _clientConnections.end());
}

int Listener::acceptConnection() {
    Connection* nconn = new Connection(_listeningSocketFd);
    _clientConnections.at(nconn->getClientSocketFd()) = nconn;
    return (nconn->getClientSocketFd());
}

void Listener::receiveRequest(int clientSocketFd) {}

Listener::~Listener() {
    if (_listeningSocketFd != -1) {
        close(_listeningSocketFd);
        _listeningSocketFd = -1;
    }
}
}  // namespace webserver
