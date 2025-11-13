#include "Connection.hpp"

#include <fcntl.h>
#include <netinet/in.h>
#include <poll.h>
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
Connection::Connection(int listeningSocketFd)
    : _clientSocket(NULL) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    _clientSocketFd =
        accept(listeningSocketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
    // TODO: maybe should store some client address info from here
    if (_clientSocketFd == -1) {
        throw runtime_error(string("accept() failed"));  // NOTE: errno here forbidden
        // TODO: probably should retry, not throw
    }
}

void Connection::setClientSocket(::pollfd* clientSocket) {
    _clientSocket = clientSocket;
}

int Connection::getClientSocketFd() const {
    return _clientSocketFd;
}

string Connection::receiveRequestContent() {
    const int BUFFER_SIZE = 4096;
    char buffer
        [BUFFER_SIZE];  // TODO: probably this should be looped in case there's more than 4096 bytes
    // TODO: check what poll actually does in these cases, maybe the rest will be just picked up on next iteration
    const ssize_t bytesRead = recv(_clientSocketFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == -1) {
        throw runtime_error(string("recv() failed"));  // NOTE: errno here forbidden
        // TODO: probably should retry, not throw
    }
    if (bytesRead == 0) {  // NOTE: we don't support keep-alive connections
        clog << "Client disconnected on socket fd " << _clientSocketFd << endl;
        close(_clientSocketFd);
        _clientSocketFd = -1;
        return ("");
    }
    buffer[bytesRead] = '\0';
    return (string(buffer));
}

void Connection::sendResponse() {
    ssize_t totalSent = 0;
    ssize_t toSend = _responseBuffer.size();
    while (totalSent < toSend) {
        ssize_t sent =
            send(_clientSocketFd, _responseBuffer.data() + totalSent, toSend - totalSent, 0);
        if (sent == -1) {
            throw runtime_error(string("send() failed"));  // NOTE: errno forbidden here
            // TODO: probably should retry, not throw
        }
        totalSent += sent;
    }
    close(_clientSocket->fd);
}

void Connection::markResponseReadyForReturn() {
    _clientSocket->events |= POLLOUT;
}

void Connection::handleRequest() {
    const string request = receiveRequestContent();
    clog << "Received request on socket fd " << _clientSocketFd << ":\n---\n" << request << "---" << endl;
    _responseBuffer = RequestHandler::handle(request);
    markResponseReadyForReturn();
    // NOTE: doesn't send directly, have to get approval from MasterListener's poll first
}

Connection::~Connection() {
    if (_clientSocketFd != -1) {
        close(_clientSocketFd);
        _clientSocketFd = -1;
    }
}
}  // namespace webserver
