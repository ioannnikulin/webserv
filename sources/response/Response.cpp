#include "Response.hpp"

#include <iostream>
#include <map>
#include <sstream>
#include <string>

#include "http_status/HttpStatus.hpp"
#include "utils/colors.hpp"
#include "utils/utils.hpp"

using std::string;

#define HTTP_PROTOCOL "HTTP/1.0"
#define SERVER_NAME "OurWebServer/1.0"

namespace webserver {

Response::Response() {
    _statusCode = HttpStatus::OK;
    _body = "";
    _headers["Content-Length"] = "0";
    _headers["Server"] = SERVER_NAME;
    _headers["Date"] = utils::getTimestamp();
}

Response::Response(int status, const std::string& body, const std::string& type)
    : _statusCode(status)
    , _body(body) {
    _headers["Content-Type"] = type;
    _headers["Content-Length"] = utils::toString(body.size());
    _headers["Server"] = SERVER_NAME;
    _headers["Date"] = utils::getTimestamp();
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

void Response::setStatus(int status) {
    _statusCode = status;
}

void Response::setBody(std::string fileContent) {
    _body = fileContent;
    _headers["Content-Length"] = utils::toString(_body.size());
}

void Response::setHeader(const std::string& key, const std::string& value) {
    _headers[key] = value;
}

string Response::serialize(void) const {
    std::clog << B_YELLOW << "Serializing the HTTP response..." << RESET_COLOR << std::endl;

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

    std::clog << B_GREEN << "✅ HTTP response serialized!" << RESET_COLOR << std::endl;

    return (resp.str());
}
}  // namespace webserver
