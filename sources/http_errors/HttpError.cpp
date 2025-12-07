#include "HttpError.hpp"

#include <map>
#include <stdexcept>
#include <string>
#include <utility>

namespace webserver {
const int HttpError::MIN_CODE = 100;
const int HttpError::MAX_CODE = 599;

std::map<int, HttpError> HttpError::_errors;

HttpError::HttpError()
    : _code(0) {
}

HttpError::HttpError(int code, const std::string& message)
    : _code(code)
    , _message(message) {
}

HttpError::HttpError(int code, const std::string& message, const std::string& page)
    : _code(code)
    , _message(message)
    , _defaultPageFileLocation(page)
    , _pageFileLocation(page) {
}

HttpError::HttpError(const HttpError& other)
    : _code(other._code)
    , _message(other._message)
    , _defaultPageFileLocation(other._defaultPageFileLocation)
    , _pageFileLocation(other._pageFileLocation) {
}

HttpError& HttpError::operator=(const HttpError& other) {
    if (this != &other) {
        _pageFileLocation = other._pageFileLocation;
        _code = other._code;
    }
    return (*this);
}

void HttpError::setPage(int code, const std::string& pageFileLocation) {
    const std::map<int, HttpError>::iterator errorIt = _errors.find(code);
    if (errorIt == _errors.end()) {
        throw std::runtime_error("Trying to set error page for unknown code");
    }

    if (pageFileLocation.empty()) {
        throw std::runtime_error("Empty page file location for error_page");
    }

    errorIt->second._pageFileLocation = pageFileLocation;
}

void HttpError::initializeErrors() {
    _errors.insert(
        std::make_pair(BAD_REQUEST, HttpError(BAD_REQUEST, "Bad Request", "/errors/400.html"))
    );

    _errors.insert(std::make_pair(FORBIDDEN, HttpError(FORBIDDEN, "Forbidden", "/errors/403.html"))
    );

    _errors.insert(std::make_pair(NOT_FOUND, HttpError(NOT_FOUND, "Not Found", "/errors/404.html"))
    );

    _errors.insert(std::make_pair(
        METHOD_NOT_ALLOWED,
        HttpError(METHOD_NOT_ALLOWED, "Method Not Allowed", "/errors/405.html")
    ));

    _errors.insert(std::make_pair(
        PAYLOAD_TOO_LARGE,
        HttpError(PAYLOAD_TOO_LARGE, "Payload Too Large", "/errors/413.html")
    ));

    _errors.insert(
        std::make_pair(I_AM_A_TEAPOT, HttpError(I_AM_A_TEAPOT, "I'm a teapot", "/errors/418.html"))
    );

    _errors.insert(std::make_pair(
        REQUEST_HEADER_FIELDS_TOO_LARGE,
        HttpError(
            REQUEST_HEADER_FIELDS_TOO_LARGE,
            "Request Header Fields Too Large",
            "/errors/431.html"
        )
    ));

    _errors.insert(std::make_pair(
        INTERNAL_SERVER_ERROR,
        HttpError(INTERNAL_SERVER_ERROR, "Internal Server Error", "/errors/500.html")
    ));

    _errors.insert(std::make_pair(
        NOT_IMPLEMENTED,
        HttpError(NOT_IMPLEMENTED, "Not Implemented", "/errors/501.html")
    ));

    _errors.insert(
        std::make_pair(BAD_GATEWAY, HttpError(BAD_GATEWAY, "Bad Gateway", "/errors/502.html"))
    );

    _errors.insert(std::make_pair(
        HTTP_VERSION_NOT_SUPPORTED,
        HttpError(HTTP_VERSION_NOT_SUPPORTED, "HTTP Version Not Supported", "/errors/505.html")
    ));
}

HttpError::~HttpError() {
}

bool HttpError::isAValidHttpStatusCode(int code) {
    return (code >= MIN_CODE && code <= MAX_CODE);
}

}  // namespace webserver
