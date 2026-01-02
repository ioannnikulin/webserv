#include "HttpStatus.hpp"

#include <map>
#include <sstream>
#include <string>
#include <utility>

using std::map;
using std::string;

namespace webserver {

const int HttpStatus::MIN_CODE = 100;
const int HttpStatus::MAX_CODE = 599;
const std::string HttpStatus::DEFAULT_STATUS_PAGE_DIR = "status_pages";

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
    oss << "./" << DEFAULT_STATUS_PAGE_DIR << "/" << code << ".html";
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
    addStatus(ACCEPTED, "Accepted");
    addStatus(NO_CONTENT, "No Content");
    addStatus(BAD_REQUEST, "Bad Request");
    addStatus(FORBIDDEN, "Forbidden");
    addStatus(NOT_FOUND, "Not Found");
    addStatus(METHOD_NOT_ALLOWED, "Method Not Allowed");
    addStatus(PAYLOAD_TOO_LARGE, "Payload Too Large");
    addStatus(I_AM_A_TEAPOT, "I am a teapot");
    addStatus(REQUEST_HEADER_FIELDS_TOO_LARGE, "Request Header Fields Too Large");
    addStatus(INTERNAL_SERVER_ERROR, "Internal Server Error");
    addStatus(NOT_IMPLEMENTED, "Not Implemented");
    addStatus(BAD_GATEWAY, "Bad Gateway");
    addStatus(HTTP_SERVICE_UNAVAILABLE, "Service Unavailable");
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
    const std::map<int, HttpStatus>::iterator itr = _statusMap.find(code);
    if (itr == _statusMap.end()) {
        throw std::runtime_error("Trying to set a status page for an unknown status code");
    }

    if (pageFileLocation.empty()) {
        throw std::runtime_error("Provided status page location is empty");
    }

    itr->second._pageFileLocation = pageFileLocation;
}

bool HttpStatus::isAValidHttpStatusCode(int code) {
    return (code >= MIN_CODE && code <= MAX_CODE);
}

}  // namespace webserver
