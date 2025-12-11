#include "HttpStatus.hpp"

#include <map>
#include <sstream>
#include <string>
#include <utility>

#include "request_handler/GetHandler.hpp"

using std::map;
using std::string;

namespace webserver {

const int HttpStatus::MIN_CODE = 100;
const int HttpStatus::MAX_CODE = 599;
HttpStatus::HttpStatus(int code, const std::string& reasonPhrase)
    : _code(code)
    , _reasonPhrase(reasonPhrase)
    , _defaultPageFileLocation(getDefaultPageLocation(code))
    , _pageFileLocation(_defaultPageFileLocation) {
}

HttpStatus::HttpStatus(int code, const std::string& reasonPhrase, const std::string& page)
    : _code(code)
    , _reasonPhrase(reasonPhrase)
    , _defaultPageFileLocation(getDefaultPageLocation(code))
    , _pageFileLocation(page) {
}

HttpStatus::HttpStatus(const HttpStatus& other)
    : _code(other._code)
    , _reasonPhrase(other._reasonPhrase)
    , _defaultPageFileLocation(other._defaultPageFileLocation)
    , _pageFileLocation(other._pageFileLocation) {
}

HttpStatus& HttpStatus::operator=(const HttpStatus& other) {
    _code = other._code;
    _reasonPhrase = other._reasonPhrase;
    _defaultPageFileLocation = other._defaultPageFileLocation;
    _pageFileLocation = other._pageFileLocation;
    return (*this);
}

HttpStatus::~HttpStatus() {
}

std::string HttpStatus::getDefaultPageLocation(int code) {
    std::ostringstream oss;
    oss << "./errors/" << code << ".html";
    return oss.str();
}

std::string HttpStatus::getPageFileLocation(int code) {
    const std::map<int, HttpStatus>::const_iterator itr = _statusMap.find(code);
    if (itr == _statusMap.end() || itr->second._pageFileLocation.empty()) {
        return getDefaultPageLocation(code);
    }
    return (itr->second._pageFileLocation);
}

std::map<int, HttpStatus> HttpStatus::_statusMap;

void HttpStatus::addStatus(int code, const std::string& reasonPhrase) {
    _statusMap.insert(
        std::make_pair(code, HttpStatus(code, reasonPhrase, getDefaultPageLocation(code)))
    );
}

void HttpStatus::initStatusMap() {
    addStatus(OK, "OK");
    addStatus(CREATED, "Created");
    addStatus(NO_CONTENT, "No Content");
    addStatus(BAD_REQUEST, "Bad Request");
    addStatus(FORBIDDEN, "Forbidden");
    addStatus(NOT_FOUND, "Not Found");
    addStatus(METHOD_NOT_ALLOWED, "Method Not Allowed");
    addStatus(PAYLOAD_TOO_LARGE, "Payload Too Large");
    addStatus(I_AM_A_TEAPOT, "I am a teapot");
    addStatus(INTERNAL_SERVER_ERROR, "Internal Server Error");
    addStatus(NOT_IMPLEMENTED, "Not Implemented");
    addStatus(BAD_GATEWAY, "Bad Gateway");
    addStatus(HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported");
}

const std::string HttpStatus::UNKNOWN_STATUS = "SERVER RESPONSE UNDEFINED";

std::string HttpStatus::getReasonPhrase(const int code) {
    const std::map<int, HttpStatus>::const_iterator itr = _statusMap.find(code);
    if (itr == _statusMap.end()) {
        return UNKNOWN_STATUS;
    }
    return (itr->second._reasonPhrase);
}

void HttpStatus::setPage(int code, const std::string& pageFileLocation) {
    const std::map<int, HttpStatus>::iterator errorIt = _statusMap.find(code);
    if (errorIt == _statusMap.end()) {
        throw std::runtime_error("Trying to set error page for unknown code");
    }

    if (pageFileLocation.empty()) {
        throw std::runtime_error("Empty page file location for error_page");
    }

    errorIt->second._pageFileLocation = pageFileLocation;
}

bool HttpStatus::isAValidHttpStatusCode(int code) {
    return (code >= MIN_CODE && code <= MAX_CODE);
}

}  // namespace webserver
