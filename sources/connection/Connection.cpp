#include "Connection.hpp"

#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <cstring>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "configuration/Endpoint.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "http_status/BadRequest.hpp"
#include "http_status/HttpException.hpp"
#include "http_status/HttpStatus.hpp"
#include "http_status/IncompleteRequest.hpp"
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

namespace {
void clean(char* buffer, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buffer[i] = 0;
    }
}
}  // namespace

namespace webserver {
Connection::Connection(int listeningSocketFd, const Endpoint& configuration)
    : _state(NEWBORN)
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
    clog << GREY << "checking [" << requestBuffer << "]" << RESET_COLOR << endl;
    try {
        const webserver::Request tmp(requestBuffer);
        return (true);
    } catch (const IncompleteRequest& e) {
        return (false);
    } catch (const BadRequest& e) {
        return (true);
    }
}

Connection::State Connection::receiveRequestContent() {
    _state = READING;
    const int READ_BUFFER_SIZE = 4096;
    char readBuffer[READ_BUFFER_SIZE];
    clean(readBuffer, READ_BUFFER_SIZE);
    ssize_t bytesRead;
    while (true) {
        bytesRead = recv(_clientSocketFd, readBuffer, sizeof(readBuffer), 0);
        if (bytesRead > 0) {
            _requestBuffer << string(readBuffer, bytesRead);
            if (fullRequestReceived()) {
                _state = READING_COMPLETE;
                return (_state);
            }
            continue;
        }
        if (bytesRead == 0) {
            // NOTE: client closed the connection himself, no way to send response
            _state = CLOSED_BY_CLIENT;
            return (_state);
        }
        // NOTE: bytesRead < 0
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return (_state);
            // NOTE: will have to finish reading later on a separate poll(), no data available currently
        }
        throw runtime_error(string("recv() failed"));
        // TODO 48: probably should retry, not throw
    }
}

void Connection::sendResponse() {
    clog << "Sending response to fd " << _clientSocketFd << endl;
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
    _state = RESPONSE_SENT;
}

Connection::State Connection::generateResponse() {
    // NOTE: called only in child process
    if (_state != READING_COMPLETE) {
        // NOTE: how did you call this? this is a wrong time to call response generator
        return (_state);
    }

    try {
        // NOTE: DL should it be cout or clog? clog is usually used for errors?
        std::clog << B_YELLOW << "Received a HTTP request on socket fd " << _clientSocketFd
                  << RESET_COLOR << ":\n---\n"
                  << _requestBuffer.str() << "---\n"
                  << endl;
        _request = Request(_requestBuffer.str());
        _responseBuffer = RequestHandler::handleRequest(_request, _configuration);
    } catch (const HttpException& e) {
        // NOTE: webserver-logic-related errors
        ostringstream oss;
        oss << "HTTP/1.0 " << e.getCode() << " " << HttpStatus::getReasonPhrase(e.getCode())
            << "\r\n"
            << "Content-Length: " << strlen(e.what()) << "\r\n"
            << "Connection: close\r\n\r\n"
            << e.what();
        _responseBuffer = oss.str();
    } catch (const exception& e) {
        // NOTE: system call failures
        ostringstream oss;
        oss << "HTTP/1.0 500 Internal Server Error\r\n"
            << "Content-Length: " << strlen(e.what()) << "\r\n"
            << "Connection: close\r\n"
            << "\r\n"
            << e.what();
        _responseBuffer = oss.str();
    }
    if (_request.getType() == SHUTDOWN) {
        _state = SERVER_SHUTTING_DOWN;
    } else {
        _state = WRITING_COMPLETE;
    }
    return (_state);
}

Connection::~Connection() {
}

}  // namespace webserver
