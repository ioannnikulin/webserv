#include "Connection.hpp"

#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "Request.hpp"
#include "RequestHandler.hpp"

using std::clog;
using std::endl;
using std::runtime_error;
using std::string;

namespace webserver {
Connection::Connection(int listeningSocketFd)
    : _clientSocket(NULL)
    , _request(NULL) {
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    _clientSocketFd =
        accept(listeningSocketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
    // TODO 53: store some client address info from here
    if (_clientSocketFd == -1) {
        throw runtime_error(string("accept() failed"));  // NOTE: errno here forbidden
        // TODO 48: probably should retry, not throw
    }
}

void Connection::setClientSocket(::pollfd* clientSocket) {
    _clientSocket = clientSocket;
}

int Connection::getClientSocketFd() const {
    return (_clientSocketFd);
}

string Connection::receiveRequestContent() {
    const int BUFFER_SIZE = 4096;
    char buffer
        [BUFFER_SIZE];  // TODO 54: probably this should be looped in case there's more than 4096 bytes
    // TODO 55: check what poll actually does in these cases, maybe the rest will be just picked up on next iteration
    const ssize_t bytesRead = recv(_clientSocketFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == -1) {
        throw runtime_error(string("recv() failed"));  // NOTE: errno here forbidden
        // TODO 48: probably should retry, not throw
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

// NOTE: DL changed all ssize_t to size_t
void Connection::sendResponse() {
    size_t totalSent = 0;
    const size_t toSend = _responseBuffer.size();
    while (totalSent < toSend) {
        const ssize_t sent =
            send(_clientSocketFd, _responseBuffer.data() + totalSent, toSend - totalSent, 0);
        if (sent == -1) {
            throw runtime_error(string("send() failed"));  // NOTE: errno forbidden here
            // TODO 48: probably should retry, not throw
        }
        totalSent += sent;
    }
    close(_clientSocket->fd);
}

void Connection::markResponseReadyForReturn() {
    _clientSocket->events |= POLLOUT;
}

void Connection::generateReponseHeaders() {
    std::ostringstream oss;
    oss << "HTTP/1.0 200 OK\r\nContent-Length: ";
    oss << _responseBuffer.size() << "\r\n\r\n";
    oss << _responseBuffer;
    _responseBuffer = oss.str();
}

void Connection::handleRequest() {
    const string rawRequest = receiveRequestContent();
    clog << "Received request on socket fd " << _clientSocketFd << ":\n---\n" << rawRequest;
    clog << "---\n" << endl;
    _request = new Request(rawRequest);
    _responseBuffer = RequestHandler::handle(_request->getType(), _request->getLocation());
    generateReponseHeaders();
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
