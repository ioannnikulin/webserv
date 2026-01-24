#include "Connection.hpp"

#include <errno.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <exception>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>

#include "cgi_handler/CgiHandler.hpp"
#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "file_system/FileSystem.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "http_status/BadRequest.hpp"
#include "http_status/HttpException.hpp"
#include "http_status/HttpStatus.hpp"
#include "http_status/IncompleteRequest.hpp"
#include "logger/Logger.hpp"
#include "request/Request.hpp"
#include "request_handler/RequestHandler.hpp"
#include "response/Response.hpp"

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

Logger Connection::_log;

Connection::Connection(int listeningSocketFd, const Endpoint& configuration)
    : _state(NEWBORN)
    , _clientIp(0)
    , _clientPort(0)
    , _configuration(configuration)
    , _route(NULL) {
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
    _log.stream(LOG_TRACE) << "Accepted connection from " << ((clientIp >> SHIFT24) & MASK8) << "."
                           << ((clientIp >> SHIFT16) & MASK8) << "."
                           << ((clientIp >> SHIFT8) & MASK8) << "." << (clientIp & MASK8) << ":"
                           << _clientPort << "\n";
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
    _log.stream(LOG_TRACE) << "checking [" + requestBuffer + "]\n";
    try {
        webserver::Request tmp(requestBuffer);
        if (!tmp.isRequestTargetReceived()) {
            return (false);
        }
        if (_route == NULL) {
            /* NOTE: this is the first time we see the first line received.
            we extract the path and try to match it to a route configuration.
            we need the body size limit here, so that we can terminate the request parsing early.
            */
            try {
                _route = &(_configuration.selectRoute(tmp.getPath()));
            } catch (const std::out_of_range& e) {
                return (true);
            }
        }

        // NOTE: we have the route already, but since we reparse the request for every check, we recast the max size into it again
        tmp.setMaxClientBodySizeBytes(_route->getFolderConfig().getMaxClientBodySizeBytes());
        tmp.getBody();  // NOTE: lazy body init

        _request = tmp;
        return (true);
    } catch (const IncompleteRequest& e) {
        return (false);
    } catch (const BadRequest& e) {
        return (true);
    }
}

bool Connection::itsACgiRequest() {
    if (_route == NULL) {
        return (false);
    }

    try {
        string path = _request.getPath();
        if (!path.empty() && path[path.length() - 1] == '/') {
            const string indexFile = _route->getFolderConfig().getIndexPageFilename();
            if (!indexFile.empty()) {
                path += indexFile;
            }
        }

        const size_t dotPos = path.find_last_of('.');

        if (dotPos == string::npos) {
            return (false);
        }

        const string extension = path.substr(dotPos);
        const std::map<string, CgiHandlerConfig*>& handlers = _configuration.getCgiHandlers();

        return (handlers.find(extension) != handlers.end());
    } catch (...) {
        return (false);
    }
}

string Connection::getRequestBody() {
    try {
        return (_request.getBody());
    } catch (...) {
        return ("");
    }
}

const CgiHandlerConfig* Connection::resolveCgiHandler(const Endpoint& config) {
    string path = _request.getPath();
    if (!path.empty() && path[path.length() - 1] == '/') {
        if (_route != NULL) {
            const string indexFile = _route->getFolderConfig().getIndexPageFilename();
            if (!indexFile.empty()) {
                path += indexFile;
            }
        }
    }

    const size_t dotPos = path.find_last_of('.');
    if (dotPos == string::npos) {
        _log.stream(LOG_ERROR) << "No extension found in CGI path\n";
        return (NULL);
    }

    const string extension = path.substr(dotPos);
    const std::map<string, CgiHandlerConfig*>& handlers = config.getCgiHandlers();

    const std::map<string, CgiHandlerConfig*>::const_iterator iter = handlers.find(extension);

    if (iter == handlers.end()) {
        _log.stream(LOG_ERROR) << "No CGI handler for extension: " << extension << "\n";
        return (NULL);
    }

    return (iter->second);
}

string Connection::resolveScriptPath() {
    std::string requestPath = _request.getPath();
    if (!requestPath.empty() && requestPath[requestPath.length() - 1] == '/') {
        const std::string indexFile = _route->getFolderConfig().getIndexPageFilename();
        if (!indexFile.empty()) {
            requestPath += indexFile;
        }
    }
    return (_route->getFolderConfig().getResolvedPath(requestPath));
}

void Connection::cgiError(const char* errorMsg) {
    if (errorMsg != NULL) {
        write(STDOUT_FILENO, errorMsg, strlen(errorMsg));
        close(STDOUT_FILENO);
        close(STDIN_FILENO);
    }

    char* argv[] = {const_cast<char*>("/usr/bin/false"), NULL};
    char* envp[] = {NULL};
    execve("/usr/bin/false", argv, envp);
    while (true) {
    }
}

Connection::State Connection::executeCgi(const Endpoint& config) {
    try {
        const CgiHandlerConfig* cgiConfig = resolveCgiHandler(config);

        if (cgiConfig == NULL) {
            cgiError(
                "Status: 500\r\nContent-Type: text/html\r\n\r\nCGI handler configuration error\r\n"
            );
        }

        if (_route == NULL) {
            _log.stream(LOG_ERROR) << "No route found for CGI request\n";
            cgiError(
                "Status: 500\r\nContent-Type: text/html\r\n\r\nNo route configuration found\r\n"
            );
        }

        const string scriptPath = resolveScriptPath();
        CgiHandler handler(*cgiConfig, _request, scriptPath, _configuration.getPort(), *_route);
        char** env = handler.prepareEnvironment();

        const string interpreterPath = handler.getExecutablePath();
        const string scriptPathStr = handler.getScriptPath();

        char* argv[] = {
            const_cast<char*>(interpreterPath.c_str()),
            const_cast<char*>(scriptPathStr.c_str()),
            // clang-format off
            NULL};
        // clang-format on

        execve(interpreterPath.c_str(), argv, env);

        _log.stream(LOG_ERROR) << "execve() failed for CGI script: " << scriptPathStr << "\n";

        for (size_t i = 0; env[i] != NULL; ++i) {
            delete[] env[i];
        }
        delete[] env;
        cgiError("Status: 500\r\nContent-Type: text/html\r\n\r\nFailed to execute CGI script\r\n");
    } catch (const std::exception& e) {
        _log.stream(LOG_ERROR) << "Exception in executeCgi: " << e.what() << "\n";
        cgiError(
            "Status: 500\r\nContent-Type: text/html\r\n\r\nInternal server error in CGI "
            "execution\r\n"
        );
    }

    return (Connection::IGNORED);  // NOTE: not returned anyway
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
                _state = itsACgiRequest() ? READING_CGI_REQUEST_COMPLETE : READING_COMPLETE;
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
    _log.stream(LOG_TRACE) << "Sending response to fd " << _clientSocketFd << "\n";
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
        _log.stream(LOG_TRACE) << "Received HTTP request on socket " << _clientSocketFd << ":\n"
                               << _requestBuffer.str();
        _request = Request(_requestBuffer.str());
        _responseBuffer = RequestHandler::handleRequest(_request, *_route);
    } catch (const HttpException& e) {
        const string pageLocation = HttpStatus::getPageFileLocation(e.getCode());
        string errorPageContent;
        try {
            errorPageContent = file_system::readFile(pageLocation.c_str());
        } catch (const exception& fileError) {
            // NOTE: fallback if custom error page cannot be loaded
            errorPageContent = e.what();
        }
        const Response response(e.getCode(), errorPageContent, "text/html");
        _responseBuffer = response.serialize();
    } catch (const exception& e) {
        const string pageLocation =
            HttpStatus::getPageFileLocation(HttpStatus::INTERNAL_SERVER_ERROR);
        string errorPageContent;
        try {
            errorPageContent = file_system::readFile(pageLocation.c_str());
        } catch (const exception& fileError) {
            // NOTE: fallback if error page cannot be loaded
            errorPageContent = e.what();
        }
        const Response response(HttpStatus::INTERNAL_SERVER_ERROR, errorPageContent, "text/html");
        _responseBuffer = response.serialize();
    }
    if (_request.getType() == SHUTDOWN) {
        _state = SERVER_SHUTTING_DOWN;
    } else {
        _state = WRITING_COMPLETE;
    }
    return (_state);
}

const Request& Connection::getRequest() const {
    return (_request);
}

Connection::~Connection() {
}

}  // namespace webserver
