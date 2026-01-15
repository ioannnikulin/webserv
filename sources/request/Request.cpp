#include "Request.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <map>
#include <sstream>
#include <string>

#include "http_methods/HttpMethodType.hpp"
#include "http_status/BadRequest.hpp"
#include "http_status/IncompleteRequest.hpp"
#include "http_status/MethodNotAllowed.hpp"
#include "http_status/PayloadTooLarge.hpp"

using std::istringstream;
using std::map;
using std::ostream;
using std::ostringstream;
using std::string;

namespace webserver {
const HttpMethodType Request::DEFAULT_TYPE = GET;
const string Request::DEFAULT_REQUEST_TARGET = "/dev/null";
const string Request::DEFAULT_HTTP_VERSION = "0.0";

size_t Request::defaultMaxClientBodySizeBytes() {
    return (std::numeric_limits<std::streamsize>::max());
}

Request::Request()
    : _method(DEFAULT_TYPE)
    , _requestTarget(DEFAULT_REQUEST_TARGET)
    , _isRequestTargetReceived(false)
    , _path(DEFAULT_REQUEST_TARGET)
    , _query("")
    , _protocolVersion(DEFAULT_HTTP_VERSION)
    , _headers()
    , _isBodyRaw(true)
    , _body("")
    , _maxClientBodySizeBytes(defaultMaxClientBodySizeBytes()) {
}

Request::Request(const Request& other)
    : _method(other._method)
    , _requestTarget(other._requestTarget)
    , _isRequestTargetReceived(other._isRequestTargetReceived)
    , _path(other._path)
    , _query(other._query)
    , _protocolVersion(other._protocolVersion)
    , _headers(other._headers)
    , _isBodyRaw(other._isBodyRaw)
    , _body(other._body)
    , _maxClientBodySizeBytes(other._maxClientBodySizeBytes) {
}

const std::string Request::MALFORMED_FIRST_LINE =
    "expected to get a first line, containing a valid request type in capitals, "
    "a requestTarget starting with '/', a protocol version starting with "
    "'HTTP/', and \\r\\n in the end";

void Request::parseChunkedBody() {
    size_t pos = 0;
    ostringstream bodybuf;
    while (true) {
        const string::size_type lineEnd = _body.find("\r\n", pos);
        if (lineEnd == string::npos) {
            throw IncompleteRequest("incomplete chunked body");
        }
        const string chunkSizeStr = _body.substr(pos, lineEnd - pos);
        istringstream iss(chunkSizeStr);
        size_t chunkSize;
        iss >> std::hex >> chunkSize;
        if (iss.fail() || !iss.eof()) {
            throw BadRequest("invalid chunk size in chunked body");
        }
        pos = lineEnd + 2;
        if (chunkSize == 0) {
            break;
        }
        if (pos + chunkSize + 2 > _body.size()) {
            throw IncompleteRequest("incomplete chunked body data");
        }
        bodybuf << _body.substr(pos, chunkSize);
        if (_body.substr(pos + chunkSize, 2) != "\r\n") {
            throw BadRequest("invalid chunk body data ending");
        }
        pos += chunkSize + 2;
        if (static_cast<size_t>(bodybuf.tellp()) > _maxClientBodySizeBytes) {
            throw PayloadTooLarge("request body exceeds maximum allowed size");
        }
    }
    _body = bodybuf.str();
}

Request::Request(string raw)
    : _method(DEFAULT_TYPE)
    , _requestTarget(DEFAULT_REQUEST_TARGET)
    , _isRequestTargetReceived(false)
    , _path(DEFAULT_REQUEST_TARGET)
    , _protocolVersion(DEFAULT_HTTP_VERSION)
    , _isBodyRaw(true)
    , _body("")
    , _maxClientBodySizeBytes(defaultMaxClientBodySizeBytes()) {
    if (raw.empty()) {
        throw IncompleteRequest("empty request");
    }
    const string::size_type endOfFirstLine = raw.find("\r\n");
    if (endOfFirstLine == string::npos) {
        if (raw.find('\n') != string::npos) {
            throw BadRequest("invalid line endings");
        }
        throw IncompleteRequest(MALFORMED_FIRST_LINE);
    }
    parseFirstLine(raw.substr(0, endOfFirstLine));
    const string::size_type endOfHeaders = raw.find("\r\n\r\n");
    if (endOfHeaders == string::npos) {
        throw IncompleteRequest("no formal end of headers");
    }
    parseHeaders(raw.substr(endOfFirstLine + 2, endOfHeaders - endOfFirstLine - 2));
    _body = (endOfHeaders != string::npos ? raw.substr(endOfHeaders + 4) : string());
}

void Request::parseBody() {
    // NOTE: do not call getBody() here, this would be a faulty recursive call
    _isBodyRaw = false;
    if (getType() != POST) {
        _body = "";
        return;
    }
    if (contentLengthSet()) {
        if (getContentLength() > _maxClientBodySizeBytes) {
            throw PayloadTooLarge("request body exceeds maximum allowed size");
        }
        const string msg = "actual received body length does not match the declared Content-Length";
        if (!contentLengthSet() || _body.size() > getContentLength()) {
            throw BadRequest(msg);
        }
        if (_body.size() < getContentLength()) {
            throw IncompleteRequest(msg);
        }
    } else if (getHeader("Transfer-Encoding") == "chunked") {
        parseChunkedBody();
    } else {
        throw BadRequest("no Content-Length or Transfer-Encoding header for POST request");
    }
}

void Request::parseFirstLine(const string& firstLine) {
    istringstream iss(firstLine);
    string method;
    if (!(iss >> method >> _requestTarget >> _protocolVersion)) {
        throw BadRequest(MALFORMED_FIRST_LINE);
    }
    try {
        _method = stringToMethod(method);
    } catch (...) {
        throw MethodNotAllowed("unsupported HTTP method: " + method);
    }
    const size_t endOfPathPosition = _requestTarget.find("?");
    if (endOfPathPosition != string::npos) {
        _path = _requestTarget.substr(0, endOfPathPosition);
        _query = _requestTarget.substr(endOfPathPosition + 1);
    } else {
        _path = _requestTarget;
    }
    _isRequestTargetReceived = true;
}

bool Request::isRequestTargetReceived() const {
    return (_isRequestTargetReceived);
}

void Request::parseHeaders(const string& rawHeaders) {
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

Request& Request::operator=(const Request& other) {
    if (this == &other) {
        return (*this);
    }
    _method = other._method;
    _requestTarget = other._requestTarget;
    _isRequestTargetReceived = other._isRequestTargetReceived;
    _path = other._path;
    _query = other._query;
    _protocolVersion = other._protocolVersion;
    _headers = other._headers;
    _body = other._body;
    _isBodyRaw = other._isBodyRaw;
    return (*this);
}

bool Request::operator==(const Request& other) {
    if (_isBodyRaw) {
        parseBody();
    }
    return (
        _method == other._method && _requestTarget == other._requestTarget &&
        _isRequestTargetReceived == other._isRequestTargetReceived &&
        _protocolVersion == other._protocolVersion && _headers == other._headers &&
        _body == other._body && _path == other._path && _query == other._query &&
        _isBodyRaw == other._isBodyRaw && _maxClientBodySizeBytes == other._maxClientBodySizeBytes
    );
}

Request& Request::setType(HttpMethodType type) {
    _method = type;
    return (*this);
}

HttpMethodType Request::getType() const {
    return (_method);
}

Request& Request::setRequestTarget(string requestTarget) {
    _isRequestTargetReceived = true;
    _requestTarget = requestTarget;
    return (*this);
}

string Request::getRequestTarget() const {
    return (_requestTarget);
}

std::string Request::getVersion() const {
    return (_protocolVersion);
}

Request& Request::setVersion(string version) {
    _protocolVersion = version;
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

bool Request::contentLengthSet() const {
    return (!getHeader("Content-Length").empty());
}

size_t Request::getContentLength() const {
    const string str = getHeader("Content-Length");
    if (str.empty()) {
        throw std::runtime_error("No Content-Length set for the request");
    }
    istringstream iss(str);
    size_t ret;
    iss >> ret;
    return (ret);
}

string Request::getBody() {
    if (_isBodyRaw) {
        parseBody();
    }
    return (_body);
}

Request& Request::setBody(std::string body) {
    _body = body;
    return (*this);
}

Request& Request::setIsBodyRaw(bool isBodyRaw) {
    _isBodyRaw = isBodyRaw;
    return (*this);
}

string Request::getPath() const {
    return (_path);
}

Request& Request::setPath(string path) {
    _path = path;
    return (*this);
}

Request::~Request() {
}

void Request::setMaxClientBodySizeBytes(size_t maxClientBodySizeBytes) {
    _maxClientBodySizeBytes = maxClientBodySizeBytes;
}

size_t Request::getMaxClientBodySizeBytes() const {
    return (_maxClientBodySizeBytes);
}

std::ostream& operator<<(std::ostream& oss, const Request& request) {
    oss << "method: " << methodToString(request._method);
    oss << " target: " << request._requestTarget;
    oss << " is target valid: " << request._isRequestTargetReceived;
    oss << " path: " << request._path;
    oss << " query: " << request._query;
    oss << " protocol: " << request._protocolVersion;
    oss << " is body raw: " << request._isBodyRaw;
    oss << " body: " << request._body << "\n";
    for (map<string, string>::const_iterator itr = request._headers.begin();
         itr != request._headers.end();
         itr++) {
        oss << itr->first << ": " << itr->second << "\n";
    }
    return (oss);
}
}  // namespace webserver
