#include "HttpStatus.hpp"

#include <map>
#include <sstream>

#include "http_methods/HttpMethodType.hpp"
#include "request_handler/GetHandler.hpp"
#include "utils/utils.hpp"

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
    std::map<int, HttpStatus>::iterator it = _statusMap.find(code);
    if (it == _statusMap.end()) {
        return;
    }
    it->second._pageFileLocation = pageFileLocation;
}

std::string HttpStatus::getPageFileLocation(int code) {
    std::map<int, HttpStatus>::const_iterator it = _statusMap.find(code);
    if (it == _statusMap.end()) {
        return getDefaultPageLocation(code);
    }
    return (it->second._pageFileLocation);
}

std::map<int, HttpStatus> HttpStatus::_statusMap;

void HttpStatus::addStatus(int code, const std::string& reasonPhrase) {
    _statusMap.insert(
        std::make_pair(code, HttpStatus(code, reasonPhrase, getDefaultPageLocation(code)))
    );
}

void HttpStatus::initStatusMap() {
    addStatus(200, "OK");
    addStatus(404, "Not Found");
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
    std::map<int, HttpStatus>::const_iterator it = _statusMap.find(code);
    if (it == _statusMap.end())
        return UNKNOWN_STATUS;
    return it->second._reasonPhrase;
}

}  // namespace webserver
