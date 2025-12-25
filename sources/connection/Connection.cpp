#include "Connection.hpp"

#include <netinet/in.h>
#include <poll.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "configuration/AppConfig.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "http_status/HttpException.hpp"
#include "http_status/HttpStatus.hpp"
#include "http_status/ShuttingDown.hpp"
#include "request/Request.hpp"
#include "request_handler/RequestHandler.hpp"
#include "utils/colors.hpp"

using std::clog;
using std::endl;
using std::exception;
using std::ostringstream;
using std::runtime_error;
using std::string;

namespace webserver {
Connection::Connection()
    : _clientSocket(NULL)
    , _clientSocketFd(0)
    , _request(NULL) {
}

Connection::Connection(int listeningSocketFd)
    : _clientSocket(NULL)
    , _request(NULL)
    , _clientIp(0)
    , _clientPort(0) {
    const uint32_t SHIFT24 = 24;
    const uint32_t SHIFT16 = 16;
    const uint32_t SHIFT8 = 8;
    const uint32_t MASK8 = 0xFF;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    _clientSocketFd =
        accept(listeningSocketFd, reinterpret_cast<struct sockaddr*>(&clientAddr), &clientAddrLen);
    if (_clientSocketFd == -1) {
        throw runtime_error(string("accept() failed"));  // NOTE: errno here forbidden
        // TODO 48: probably should retry, not throw
    }
    _clientIp = clientAddr.sin_addr.s_addr;
    _clientPort = ntohs(clientAddr.sin_port);

    const uint32_t clientIp = ntohl(_clientIp);
    clog << "Accepted connection from " << ((clientIp >> SHIFT24) & MASK8) << "."
         << ((clientIp >> SHIFT16) & MASK8) << "." << ((clientIp >> SHIFT8) & MASK8) << "."
         << (clientIp & MASK8) << ":" << _clientPort << endl;
}

Connection& Connection::setClientSocket(::pollfd* clientSocket) {
    _clientSocket = clientSocket;
    return (*this);
}

Connection& Connection::setResponseBuffer(string buffer) {
    _responseBuffer = buffer;
    return (*this);
}

std::string Connection::getResponseBuffer() const {
    return (_responseBuffer);
}

int Connection::getClientSocketFd() const {
    return (_clientSocketFd);
}

string Connection::receiveRequestContent() {
    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];
    // TODO 54: probably this should be looped in case there's more than 4096 bytes
    // TODO 55: check what poll actually does in these cases, maybe the rest will be just picked up on next iteration
    const ssize_t bytesRead = recv(_clientSocketFd, buffer, sizeof(buffer) - 1, 0);
    if (bytesRead == -1) {
        throw runtime_error(string("recv() failed"));
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

void Connection::sendResponse() {
    size_t totalSent = 0;
    const size_t toSend = _responseBuffer.size();
    while (totalSent < toSend) {
        const ssize_t sent =
            send(_clientSocketFd, _responseBuffer.data() + totalSent, toSend - totalSent, 0);
        if (sent == -1) {
            throw runtime_error(string("send() failed"));
            // TODO 48: probably should retry, not throw
        }
        totalSent += sent;
    }
    close(_clientSocket->fd);
}

void Connection::markResponseReadyForReturn() {
    _clientSocket->events |= POLLOUT;
}

void Connection::handleRequest(const AppConfig* appConfig, bool shouldDeny) {
    bool requestTermination = false;
    try {
        const string rawRequest = receiveRequestContent();
        // NOTE: DL should it be cout or clog? clog is usually used for errors?
        std::clog << B_YELLOW << "Received a HTTP request on socket fd " << _clientSocketFd << RESET
                  << ":\n---\n"
                  << rawRequest << "---\n"
                  << endl;
        if (shouldDeny) {
            throw ShuttingDown();
            // NOTE: another request said to shut down, we delegated handling here to reuse the response
        }
        _request = new Request(rawRequest);
        if (_request->getType() == SHUTDOWN) {
            throw ShuttingDown();
        }
        _responseBuffer = RequestHandler::handleRequest(_request, appConfig);
        // NOTE: doesn't send directly, have to get approval from MasterListener's poll first
    } catch (const HttpException& e) {
        ostringstream oss;
        oss << "HTTP/1.0 " << e.getCode() << " " << HttpStatus::getReasonPhrase(e.getCode())
            << "\r\n"
            << "Content-Length: " << strlen(e.what()) << "\r\n"
            << "Connection: close\r\n\r\n"
            << e.what();
        _responseBuffer = oss.str();
    } catch (const ShuttingDown& e) {
        _responseBuffer =
            "HTTP/1.0 503 Service Unavailable\r\n"
            "Content-Length: 19\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Server is shutting down";
        requestTermination = true;
    } catch (const exception& e) {
        _responseBuffer =
            "HTTP/1.0 500 Internal Server Error\r\n"
            "Content-Length: 21\r\n"
            "Connection: close\r\n"
            "\r\n"
            "Internal Server Error";
    }
    markResponseReadyForReturn();
    if (requestTermination) {
        throw(ShuttingDown());
    }
}

Connection::~Connection() {
    if (_clientSocketFd != -1) {
        close(_clientSocketFd);
        _clientSocketFd = -1;
    }
}
}  // namespace webserver
