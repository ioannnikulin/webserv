#include "HttpStatus.hpp"

#include <map>
#include <sstream>
#include <string>
#include <utility>

#include "request_handler/GetHandler.hpp"

using std::map;
using std::string;

namespace webserver {
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

void HttpStatus::setPage(int code, const std::string& pageFileLocation) {
    const std::map<int, HttpStatus>::iterator itr = _statusMap.find(code);
    if (itr == _statusMap.end()) {
        return;
    }
    itr->second._pageFileLocation = pageFileLocation;
}

std::string HttpStatus::getPageFileLocation(int code) {
    const std::map<int, HttpStatus>::const_iterator itr = _statusMap.find(code);
    if (itr == _statusMap.end()) {
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
    addStatus(200, "OK");         // NOLINT(readability-magic-numbers)
    addStatus(404, "Not Found");  // NOLINT(readability-magic-numbers)
    /* NOTE: error codes to be added later
    addStatus(201, "Created");
    addStatus(204, "No Content");
    addStatus(400, "Bad Request");
    addStatus(403, "Forbidden");
    addStatus(405, "Method Not Allowed");
    addStatus(413, "Payload Too Large");
    addStatus(500, "Internal Server Error");
    addStatus(501, "Not Implemented");
    addStatus(505, "HTTP Version Not Supported");
	*/
}

const std::string HttpStatus::UNKNOWN_STATUS = "SERVER RESPONSE UNDEFINED";

std::string HttpStatus::getReasonPhrase(const int code) {
    const std::map<int, HttpStatus>::const_iterator itr = _statusMap.find(code);
    if (itr == _statusMap.end()) {
        return UNKNOWN_STATUS;
    }
    return (itr->second._reasonPhrase);
}

}  // namespace webserver
