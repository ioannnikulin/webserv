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
    : _type(DEFAULT_TYPE)
    , _location(DEFAULT_LOCATION)
    , _protocol(DEFAULT_PROTOCOL) {
}

Request::Request(const Request& other)
    : _type(other._type)
    , _location(other._location)
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
    string::size_type endOfFirstLine = raw.find("\r\n");
    if (endOfFirstLine == string::npos) {
        if (raw.find('\n') != string::npos) {
            throw BadRequest("invalid line endings");
        }
        throw BadRequest(MALFORMED_FIRST_LINE);
    }
    parseFirstLine(raw.substr(0, endOfFirstLine));
    string::size_type endOfHeaders = raw.find("\r\n\r\n");
    parseHeaders(raw.substr(endOfFirstLine + 2, endOfHeaders - endOfFirstLine - 2));
    parseBody(endOfHeaders != string::npos ? raw.substr(endOfHeaders + 4) : string());
}

void Request::parseFirstLine(std::string firstLine) {
    istringstream iss(firstLine);
    if (!(iss >> _type >> _location >> _protocol)) {
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
        string::size_type colon = rawHeaders.find(':', lineStart);
        if (colon == string::npos || colon > lineEnd) {
            throw BadRequest("no colon in a header line");
        }
        string key = rawHeaders.substr(lineStart, colon - lineStart);
        // skip spaces after colon
        string::size_type valueStart = colon + 1;
        while (valueStart < lineEnd && rawHeaders[valueStart] == ' ') {
            valueStart++;
        }
        string value = rawHeaders.substr(valueStart, lineEnd - valueStart);
        _headers[key] = value;
        lineStart = lineEnd + 2;
    }
}

void Request::parseBody(std::string body) {
    _body = body;
}

bool Request::operator==(const Request& other) const {
    return (
        _type == other._type && _location == other._location && _protocol == other._protocol &&
        _headers == other._headers && _body == other._body
    );
}

Request& Request::setType(string type) {
    _type = type;
    return (*this);
}

string Request::getType() const {
    return (_type);
}

Request& Request::setLocation(string location) {
    _location = location;
    return (*this);
}

string Request::getLocation() const {
    return (_location);
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
    map<string, string>::const_iterator it = _headers.find(key);
    if (it != _headers.end()) {
        return it->second;
    }
    return "";
}

Request::~Request() {
}

}  // namespace webserver
