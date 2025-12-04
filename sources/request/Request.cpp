#include "Request.hpp"

#include <string>

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
    , _headers(other._headers) {
}

Request::Request(std::string raw)
    : _type("GET")
    , _location("/")
    , _protocol("HTTP/1.1") {
    (void)raw;
    _headers["Host"] = "localhost:8888";
    _headers["User-Agent"] = "curl/8.5.0";
    _headers["Accept"] = "*/*";
}

bool Request::operator==(const Request& other) const {
    return (
        _type == other._type && _location == other._location && _protocol == other._protocol &&
        _headers == other._headers
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

Request::~Request() {
}

}  // namespace webserver
