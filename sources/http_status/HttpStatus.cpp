#include "HttpStatus.hpp"

#include <stdlib.h>

#include <cstring>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

#include "file_system/FileSystem.hpp"
#include "file_system/MimeType.hpp"
#include "logger/Logger.hpp"
#include "response/Response.hpp"

using std::map;
using std::ostream;
using std::string;

namespace webserver {

const int HttpStatus::MIN_CODE = 100;
const int HttpStatus::MAX_CODE = 599;
std::string HttpStatus::Item::defaultFolder() {
    return ("status_pages");
    // NOTE: cannot be static constant because of initialization order issues
}

HttpStatus::Item::Item()
    : _code(0)
    , _reasonPhrase("")
    , _pageFileLocation(getDefaultPageFileLocation()) {
}

HttpStatus::Item::Item(int code, const string& reasonPhrase)
    : _code(code)
    , _reasonPhrase(reasonPhrase)
    , _pageFileLocation(getDefaultPageFileLocation()) {
}

HttpStatus::Item::Item(int code, const string& reasonPhrase, const string& page)
    : _code(code)
    , _reasonPhrase(reasonPhrase)
    , _pageFileLocation(page) {
}

HttpStatus::Item::Item(const Item& other)
    : _code(other._code)
    , _reasonPhrase(other._reasonPhrase)
    , _pageFileLocation(other._pageFileLocation) {
}

HttpStatus::Item& HttpStatus::Item::operator=(const HttpStatus::Item& other) {
    if (this == &other) {
        return (*this);
    }
    _code = other._code;
    _reasonPhrase = other._reasonPhrase;
    _pageFileLocation = other._pageFileLocation;
    return (*this);
}

bool HttpStatus::Item::operator==(const Item& other) const {
    return (
        _code == other._code && _reasonPhrase == other._reasonPhrase &&
        _pageFileLocation == other._pageFileLocation
    );
}

bool HttpStatus::Item::operator<(const Item& other) const {
    return (_code < other._code);
}

HttpStatus::Item::~Item() {
}

HttpStatus::HttpStatus(const HttpStatus& other)
    : _statusMap(other._statusMap) {
    Logger log;
    log.stream(LOG_TRACE) << "copying\n";
}

HttpStatus& HttpStatus::operator=(const HttpStatus& other) {
    if (this == &other) {
        return (*this);
    }
    _statusMap = other._statusMap;
    Logger log;
    log.stream(LOG_TRACE) << "assigning\n";
    return (*this);
}

bool HttpStatus::operator==(const HttpStatus& other) const {
    return (_statusMap == other._statusMap);
}

bool HttpStatus::operator!=(const HttpStatus& other) const {
    return (!(*this == other));
}

HttpStatus::~HttpStatus() {
}

const string& HttpStatus::Item::getPageFileLocation() const {
    return (_pageFileLocation);
}

string HttpStatus::Item::getDefaultPageFileLocation() const {
    std::ostringstream oss;
    oss << "./" << defaultFolder() << "/" << _code << ".html";
    const string res = oss.str();
    return (res);
}

const string& HttpStatus::Item::getReasonPhrase() const {
    return (_reasonPhrase);
}

const string& HttpStatus::getPageFileLocation(int code) const {
    Logger log;
    log.stream(LOG_TRACE) << "searching " << _statusMap.size() << " entries\n";
    const std::map<int, Item>::const_iterator itr = _statusMap.find(code);
    if (itr == _statusMap.end()) {
        throw std::out_of_range("no such code");
    }
    return (itr->second.getPageFileLocation());
}

void HttpStatus::addStatus(map<int, Item>& map, int code, const string& reasonPhrase) {
    map.insert(std::make_pair(code, Item(code, reasonPhrase)));
}

const map<int, HttpStatus::Item>& HttpStatus::defaultStatusMap() {
    static const map<int, HttpStatus::Item> map = createDefaultStatusMap();
    return (map);
}

map<int, HttpStatus::Item> HttpStatus::createDefaultStatusMap() {
    map<int, HttpStatus::Item> res;
    addStatus(res, OK, "OK");
    addStatus(res, CREATED, "Created");
    addStatus(res, ACCEPTED, "Accepted");
    addStatus(res, NO_CONTENT, "No Content");
    addStatus(res, MOVED_PERMANENTLY, "Moved permanently");
    addStatus(res, BAD_REQUEST, "Bad Request");
    addStatus(res, FORBIDDEN, "Forbidden");
    addStatus(res, NOT_FOUND, "Not Found");
    addStatus(res, METHOD_NOT_ALLOWED, "Method Not Allowed");
    addStatus(res, PAYLOAD_TOO_LARGE, "Payload Too Large");
    addStatus(res, I_AM_A_TEAPOT, "I am a teapot");
    addStatus(res, REQUEST_HEADER_FIELDS_TOO_LARGE, "Request Header Fields Too Large");
    addStatus(res, INTERNAL_SERVER_ERROR, "Internal Server Error");
    addStatus(res, NOT_IMPLEMENTED, "Not Implemented");
    addStatus(res, BAD_GATEWAY, "Bad Gateway");
    addStatus(res, HTTP_SERVICE_UNAVAILABLE, "Service Unavailable");
    addStatus(res, GATEWAY_TIMEOUT, "Gateway Timeout");
    addStatus(res, HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported");
    return (res);
}

HttpStatus::HttpStatus()
    : _statusMap(defaultStatusMap()) {
    Logger log;
    log.stream(LOG_TRACE) << "init " << _statusMap.size() << "\n";
}

const string HttpStatus::UNKNOWN_STATUS = "SERVER RESPONSE UNDEFINED";

string HttpStatus::getReasonPhrase(int code) const {
    Logger log;
    log.stream(LOG_TRACE) << "searching " << _statusMap.size() << " entries in " << &_statusMap
                          << "\n";
    const std::map<int, Item>::const_iterator itr = _statusMap.find(code);
    if (itr == _statusMap.end()) {
        return (UNKNOWN_STATUS);
    }
    return (itr->second.getReasonPhrase());
}

HttpStatus& HttpStatus::setPage(int code, const string& pageFileLocation) {
    const std::map<int, Item>::iterator itr = _statusMap.find(code);
    if (itr == _statusMap.end()) {
        throw std::out_of_range("Trying to set a status page for an unknown status code");
    }

    if (pageFileLocation.empty()) {
        throw std::runtime_error("Provided status page location is empty");
    }

    itr->second.setPageFileLocation(pageFileLocation);
    return (*this);
}

HttpStatus::Item& HttpStatus::Item::setPageFileLocation(string location) {
    _pageFileLocation = location;
    return (*this);
}

bool HttpStatus::isAValidHttpStatusCode(int code) {
    map<int, HttpStatus::Item> local = createDefaultStatusMap();
    return (local.find(code) != local.end());
}

Response HttpStatus::serveStatusPage(int statusCode, string reasonPhrase, string uncheckedPath) {
    const Logger log;
    if (!file_system::fileExists(uncheckedPath.c_str())) {
        webserver::Logger log;
        log.stream(LOG_WARN) << "Status page file not found: " << uncheckedPath
                             << ". Serving default message.\n";
        return (Response(statusCode, reasonPhrase, reasonPhrase, MimeType::getMimeType("html")));
    }
    return (file_system::serveFile(uncheckedPath, statusCode, reasonPhrase));
}

Response HttpStatus::serveStatusPage(int statusCode) const {
    return (
        serveStatusPage(statusCode, getReasonPhrase(statusCode), getPageFileLocation(statusCode))
    );
}

Response HttpStatus::ultimateInternalServerError() {
    const Item status = defaultStatusMap().at(INTERNAL_SERVER_ERROR);
    return (serveStatusPage(
        INTERNAL_SERVER_ERROR,
        status.getReasonPhrase(),
        status.getPageFileLocation()
    ));
}

ostream& operator<<(ostream& oss, const HttpStatus& catalogue) {
    for (map<int, HttpStatus::Item>::const_iterator itr = catalogue._statusMap.begin();
         itr != catalogue._statusMap.end();
         itr++) {
        oss << itr->first << ": ";
        itr->second.print(oss);
    }
    return (oss);
}

void HttpStatus::Item::print(ostream& oss) const {
    oss << _code << " " << _reasonPhrase << " " << _pageFileLocation << "\n";
}
}  // namespace webserver
