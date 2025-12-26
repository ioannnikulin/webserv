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
using std::stringstream;

namespace webserver {
Connection::Connection(int listeningSocketFd, const Endpoint& configuration)
    : _state(NEWBORN)
    , _clientSocket(NULL)
    , _clientIp(0)
    , _clientPort(0) 
    , _configuration(configuration) {
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

bool Connection::fullRequestReceived() {
    const string requestBuffer = _requestBuffer.str();
    clog << "checking [" << requestBuffer << "]" << endl;
    const size_t pos = requestBuffer.find("\r\n\r\n");
    if (pos == string::npos) {
        return (false);
    }
    // NOTE: we have \r\n\r\n. for GET and DELETE this means the request is complete. for POST we have to check the body size.
    try {
        const webserver::Request tmp(requestBuffer);
        if (tmp.getType() == POST) {
            if (!tmp.contentLengthSet() || tmp.getContentLength() != tmp.getBody().length()) {
                return (false);
            }
            return (true);
        }
        return (true);
    } catch (exception& e) {
        // NOTE: if the request wasn't parsed correctly, let's consider it complete to report an error on it
        return (false);
    }
}

void Connection::receiveRequestContent() {
    _state = READING;
    const int READ_BUFFER_SIZE = 4096;
    char readBuffer[READ_BUFFER_SIZE];
    const ssize_t bytesRead = recv(_clientSocket->fd, readBuffer, sizeof(readBuffer), 0);
    if (bytesRead == -1) {
        throw runtime_error(string("recv() failed"));
        // TODO 48: probably should retry, not throw
    }
    if (bytesRead == 0) {
        throw TerminatedByClient();
    } else {
        _requestBuffer << string(readBuffer, bytesRead);
        if (fullRequestReceived()) {
            _state = WRITING;
        }
    }
}

void Connection::sendResponse() {
    clog << "Sending response to fd " << _clientSocket->fd << endl; 
    size_t totalSent = 0;
    const size_t toSend = _responseBuffer.size();
    while (totalSent < toSend) {
        const ssize_t sent =
            send(_clientSocket->fd, _responseBuffer.data() + totalSent, toSend - totalSent, 0);
        if (sent == -1) {
            throw runtime_error(string("send() failed"));
            // TODO 48: probably should retry, not throw
        }
        totalSent += sent;
    }
    close(_clientSocket->fd);
}

void Connection::markResponseReadyForReturn() {
    clog << "Marked data on fd " << _clientSocket->fd << " ready for return" << endl;
    _clientSocket->events |= POLLOUT;
}

void Connection::handleRequest(bool shouldDeny) {
    receiveRequestContent();
    if (_state == READING) {
        return;  // NOTE: still reading, wait for next poll
    }

    bool requestTermination = false;
    try {
        // NOTE: DL should it be cout or clog? clog is usually used for errors?
        std::clog << B_YELLOW << "Received a HTTP request on socket fd " << _clientSocket->fd
                  << RESET << ":\n---\n"
                  << _requestBuffer.str() << "---\n"
                  << endl;
        if (shouldDeny) {
            throw ShuttingDown();
            // NOTE: another request said to shut down, we delegated handling here to reuse the response
        }
        _request = Request(_requestBuffer.str());
        if (_request.getType() == SHUTDOWN) {
            _state = CLOSED;
            throw ShuttingDown();
        }
        _responseBuffer = RequestHandler::handleRequest(_request, _configuration);
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
        throw ShuttingDown();
    }
}

Connection::~Connection() {
    if (_clientSocket->fd != -1) {
        close(_clientSocket->fd);
        _clientSocket->fd = -1;
    }
}

Connection::TerminatedByClient::TerminatedByClient() {
}

Connection::TerminatedByClient::~TerminatedByClient() throw() {
}

const char* Connection::TerminatedByClient::what() const throw() {
    return ("Client terminated the connection");
}
}  // namespace webserver
