#include "Listener.hpp"

#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

#include "configuration/Endpoint.hpp"
#include "logger/Logger.hpp"

using std::map;
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
    // NOTE: not sure, subject forbids this flag for MacOS
    // NOTE: for MacOS, fcntl() can only be used with: F_SETFL, O_NONBLOCK, FD_CLOEXEC
    const int flags = fcntl(socketFd, F_GETFL, 0);
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

Logger Listener::_log;

Listener& Listener::operator=(const Listener& other) {
    _log.stream(LOG_WARN) << "Unexpected stub assignment operator called for Listener\n";
    _port = other._port;
    _interface = other._interface;
    _listeningSocketFd = 0;
    _clientConnections = other._clientConnections;
    return (*this);
}

struct ::sockaddr_in Listener::resolveAddress() const {
    struct addrinfo hints;
    struct addrinfo* res = NULL;

    hints.ai_family = AF_UNSPEC;      // NOTE: AF_INET or AF_INET6
    hints.ai_socktype = SOCK_STREAM;  // NOTE: TCP
    hints.ai_flags = AI_PASSIVE;      // NOTE: for bind()
    hints.ai_protocol = 0;
    hints.ai_addrlen = 0;
    hints.ai_canonname = 0;
    hints.ai_addr = 0;
    hints.ai_next = 0;

    std::ostringstream oss;
    oss << _port;
    const std::string portStr = oss.str();

    const char* node = (_interface == "*" ? NULL : _interface.c_str());

    const int err = getaddrinfo(node, portStr.c_str(), &hints, &res);
    if (err != 0) {
        throw runtime_error(string("getaddrinfo() failed for interface: ") + _interface);
    }
    struct ::sockaddr_in addr;
    struct ::sockaddr_in const* resolved =
        reinterpret_cast<struct sockaddr_in const*>(res->ai_addr);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(_port);
    addr.sin_addr = resolved->sin_addr;

    freeaddrinfo(res);
    return (addr);
}

Listener::Listener(const Endpoint& configuration)
    : _interface(configuration.getInterface())
    , _port(configuration.getPort())
    , _listeningSocketFd(setupSocket())
    , _configuration(configuration) {
    struct sockaddr_in addr = resolveAddress();

    /* NOTE:
    probably this is wrong - we're taking the socket before the server actually starts up,
    and report about listening on this socket before calling start().
    should separate preparation from actual binding to sockets
	*/
    if (bind(_listeningSocketFd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1) {
        close(_listeningSocketFd);
        throw runtime_error(string("bind() failed: ") + strerror(errno));
    }

    if (listen(_listeningSocketFd, SOMAXCONN) == -1) {
        close(_listeningSocketFd);
        throw runtime_error(string("listen() failed: ") + strerror(errno));
    }
    // clang-format off
    _log.stream(LOG_INFO) << "Listener initialized on " << "http://" << _interface << ":" << _port << " via socket " << _listeningSocketFd << "\n";
    // clang-format on
}

string Listener::getResponse(int clientSocketFd) const {
    return (_clientConnections.at(clientSocketFd)->getResponseBuffer());
}

Listener& Listener::setResponse(int clientSocketFd, string response) {
    _clientConnections.at(clientSocketFd)->setResponseBuffer(response);
    return (*this);
}

Request Listener::getRequestFor(int clientSocketFd) const {
	return (_clientConnections.at(clientSocketFd)->getRequest());
}

int Listener::getListeningSocketFd() const {
    return (_listeningSocketFd);
}

bool Listener::hasActiveClientSocket(int clientSocketFd) const {
    return (_clientConnections.find(clientSocketFd) != _clientConnections.end());
}

int Listener::acceptConnection() {
    Connection* nconn = new Connection(_listeningSocketFd, _configuration);
    _clientConnections[nconn->getClientSocketFd()] = nconn;
    return (nconn->getClientSocketFd());
}

Connection::State Listener::receiveRequest(int clientSocketFd) {
    return (_clientConnections.at(clientSocketFd)->receiveRequestContent());
}

Connection::State Listener::generateResponse(int clientSocketFd) {
    return (_clientConnections.at(clientSocketFd)->generateResponse());
}

void Listener::sendResponse(int clientSocketFd) {
    _clientConnections.at(clientSocketFd)->sendResponse();
}

void Listener::killConnection(int clientSocketFd) {
    _log.stream(LOG_INFO) << "Killing connection\n";
    close(clientSocketFd);
    const map<int, Connection*>::iterator itr = _clientConnections.find(clientSocketFd);
    delete itr->second;
    _clientConnections.erase(itr);
}

Connection::State Listener::executeCgi(int clientSocketFd) {
    return (_clientConnections.at(clientSocketFd)->executeCgi(_configuration));
}

std::string Listener::getRequestBody(int clientSocketFd) {
    return (_clientConnections.at(clientSocketFd)->getRequestBody());
}

Listener::~Listener() {
    if (_listeningSocketFd != -1) {
        close(_listeningSocketFd);
        _listeningSocketFd = -1;
    }
    _log.stream(LOG_TRACE) << "Destroyed listener\n";
}
}  // namespace webserver
