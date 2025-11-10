#include "Connection.hpp"

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

#include "RequestHandler.hpp"

using std::cerr;
using std::clog;
using std::endl;
using std::runtime_error;
using std::strerror;
using std::string;

namespace webserver {
Connection::Connection(int listeningSocketFd) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    _clientSocketFd =
        accept(listeningSocketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
    if (_clientSocketFd == -1) {
        throw runtime_error(string("accept() failed: ") + strerror(errno));  // not sure
    }
}

int Connection::getClientSocketFd() const {
    return _clientSocketFd;
}

string Connection::receiveRequestContent() {
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];  // probably this should be looped
    const ssize_t bytesRead = recv(_clientSocketFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == -1) {
        throw runtime_error(string("recv() failed: ") + strerror(errno));
    }
    if (bytesRead == 0) {
        clog << "Client disconnected on socket fd " << _clientSocketFd << endl;
        close(_clientSocketFd);
        _clientSocketFd = -1;
        return;
    }
    buffer[bytesRead] = '\0';
    return (string(buffer));
}

void Connection::sendResponse(string response) const {
    ssize_t totalSent = 0;
    ssize_t toSend = response.size();
    while (totalSent < toSend) {
        ssize_t sent = send(_clientSocketFd, response.data() + totalSent, toSend - totalSent, 0);
        if (sent == -1) {
            throw runtime_error(string("send() failed: ") + strerror(errno));
        }
        totalSent += sent;
    }
}

void Connection::handleRequest() {
    const string request = receiveRequestContent();
    clog << "Received request on socket fd " << _clientSocketFd << ":\n" << request << endl;
    const string response = RequestHandler::handle(request);
    sendResponse(response);
}

Connection::~Connection() {
    if (_clientSocketFd != -1) {
        close(_clientSocketFd);
        _clientSocketFd = -1;
    }
}
}  // namespace webserver