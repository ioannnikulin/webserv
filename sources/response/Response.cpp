#include "Response.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "http_status/HttpStatus.hpp"
#include "logger/Logger.hpp"
#include "utils/utils.hpp"

using std::string;

namespace webserver {

Logger Response::_log;

Response::Response() {
    _statusCode = HttpStatus::OK;
    _body = "";
    _headers["Content-Length"] = "0";
    _headers["Server"] = SERVER_NAME;
    _headers["Date"] = utils::getTimestamp();
    _headers["Connection"] = "close";
}

Response::Response(int status, const std::string& body, const std::string& type)
    : _statusCode(status)
    , _body(body) {
    _headers["Content-Type"] = type;
    _headers["Content-Length"] = utils::toString(body.size());
    _headers["Server"] = SERVER_NAME;
    _headers["Date"] = utils::getTimestamp();
    _headers["Connection"] = "close";
}

Response::Response(const Response& other)
    : _statusCode(other._statusCode)
    , _headers(other._headers)
    , _body(other._body) {
}

Response& Response::operator=(const Response& other) {
    if (this != &other) {
        _statusCode = other._statusCode;
        _body = other._body;
        _headers = other._headers;
    }
    return (*this);
}

Response::~Response() {
}

int Response::getStatus() const {
    return (_statusCode);
}

const std::string& Response::getBody() const {
    return (_body);
}

std::string Response::getHeader(const std::string& key) const {
    const std::map<std::string, std::string>::const_iterator itr = _headers.find(key);
    if (itr != _headers.end()) {
        return (itr->second);
    }
    return ("");
}

Response& Response::setStatus(int status) {
    _statusCode = status;
    return (*this);
}

Response& Response::setBody(std::string fileContent) {
    _body = fileContent;
    _headers["Content-Length"] = utils::toString(_body.size());
    return (*this);
}

Response& Response::setHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
    return (*this);
}

string Response::serialize(void) const {
    _log.stream(LOG_TRACE) << "Serializing HTTP response\n";

    std::ostringstream resp;

    // NOTE: STATUS LINE
    resp << HTTP_PROTOCOL << " " << _statusCode << " ";
    resp << HttpStatus::getReasonPhrase(_statusCode) << "\r\n";

    // NOTE: CUSTOM HEADERS (if you store them in a map)
    for (std::map<std::string, std::string>::const_iterator it = _headers.begin();
         it != _headers.end();
         ++it) {
        resp << it->first << ": " << it->second << "\r\n";
    }
    // NOTE: END OF HEADERS
    resp << "\r\n";

    // NOTE: BODY
    resp << _body;

    _log.stream(LOG_TRACE) << "HTTP response serialized\n";

    return (resp.str());
}
}  // namespace webserver
