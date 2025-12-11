#include "Request.hpp"

#include <map>
#include <sstream>
#include <string>

#include "http_status/BadRequest.hpp"

using std::istringstream;
using std::map;
using std::string;

namespace webserver {
const string Request::DEFAULT_TYPE = "INVALID";
const string Request::DEFAULT_LOCATION = "/dev/null";
const string Request::DEFAULT_PROTOCOL = "0.0";

Request::Request()
    : _method(DEFAULT_TYPE)
    , _requestTarget(DEFAULT_LOCATION)
    , _protocol(DEFAULT_PROTOCOL) {
}

Request::Request(const Request& other)
    : _method(other._method)
    , _requestTarget(other._requestTarget)
    , _protocol(other._protocol)
    , _headers(other._headers)
    , _body(other._body) {
}

const std::string Request::MALFORMED_FIRST_LINE =
    "expected to get a first line, containing a valid request type in capitals, "
    "a location starting with '/', a protocol version starting with "
    "'HTTP/', and \r\n in the end";

Request::Request(string raw) {
    if (raw.empty()) {
        throw BadRequest("empty request");
    }
    const string::size_type endOfFirstLine = raw.find("\r\n");
    if (endOfFirstLine == string::npos) {
        if (raw.find('\n') != string::npos) {
            throw BadRequest("invalid line endings");
        }
        throw BadRequest(MALFORMED_FIRST_LINE);
    }
    parseFirstLine(raw.substr(0, endOfFirstLine));
    const string::size_type endOfHeaders = raw.find("\r\n\r\n");
    parseHeaders(raw.substr(endOfFirstLine + 2, endOfHeaders - endOfFirstLine - 2));
    parseBody(endOfHeaders != string::npos ? raw.substr(endOfHeaders + 4) : string());
}

void Request::parseFirstLine(std::string firstLine) {
    istringstream iss(firstLine);
    if (!(iss >> _method >> _requestTarget >> _protocol)) {
        throw BadRequest(MALFORMED_FIRST_LINE);
    }
}

void Request::parseHeaders(std::string rawHeaders) {
    string::size_type lineStart = 0;
    while (lineStart < rawHeaders.size()) {
        string::size_type lineEnd = rawHeaders.find("\r\n", lineStart);
        if (lineEnd == string::npos) {
            lineEnd = rawHeaders.size();
        }
        const string::size_type colon = rawHeaders.find(':', lineStart);
        if (colon == string::npos || colon > lineEnd) {
            throw BadRequest("no colon in a header line");
        }
        const string key = rawHeaders.substr(lineStart, colon - lineStart);
        // NOTE: skip spaces after colon
        string::size_type valueStart = colon + 1;
        while (valueStart < lineEnd && rawHeaders[valueStart] == ' ') {
            valueStart++;
        }
        const string value = rawHeaders.substr(valueStart, lineEnd - valueStart);
        _headers[key] = value;
        lineStart = lineEnd + 2;
    }
}

void Request::parseBody(std::string body) {
    _body = body;
}

bool Request::operator==(const Request& other) const {
    return (
        _method == other._method && _requestTarget == other._requestTarget &&
        _protocol == other._protocol && _headers == other._headers && _body == other._body
    );
}

Request& Request::setType(string type) {
    _method = type;
    return (*this);
}

string Request::getType() const {
    return (_method);
}

Request& Request::setRequestTarget(string location) {
    _requestTarget = location;
    return (*this);
}

string Request::getRequestTarget() const {
    return (_requestTarget);
}

Request& Request::setProtocol(string protocol) {
    _protocol = protocol;
    return (*this);
}

Request& Request::addHeader(string key, string value) {
    _headers[key] = value;
    return (*this);
}

string Request::getHeader(std::string key) const {
    const map<string, string>::const_iterator itr = _headers.find(key);
    if (itr != _headers.end()) {
        return (itr->second);
    }
    return ("");
}

Request::~Request() {
}

}  // namespace webserver
