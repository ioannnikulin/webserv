#include "http_status/HttpException.hpp"

#include <sstream>
#include <string>

#include "http_status/BadRequest.hpp"
#include "http_status/HttpStatus.hpp"
#include "http_status/IncompleteRequest.hpp"
#include "http_status/PayloadTooLarge.hpp"
#include "http_status/ShuttingDown.hpp"

using std::ostringstream;
using std::string;

namespace webserver {
const char* HttpException::what() const throw() {
    ostringstream oss;
    oss << HttpStatus::getReasonPhrase(_code) << ": " << _message;
    _resultBuffer = oss.str();
    return (_resultBuffer.c_str());
}

HttpStatus::CODE HttpException::getCode() const {
    return (_code);
}

HttpException::HttpException(HttpStatus::CODE code, std::string message)
    : _code(code)
    , _message(message) {
}

HttpException::HttpException(const HttpException& other) {
    if (this == &other) {
        return;
    }
    _code = other._code;
    _message = other._message;
}

HttpException::~HttpException() throw() {
}

BadRequest::BadRequest(string message)
    : HttpException(HttpStatus::BAD_REQUEST, message) {
}

BadRequest::BadRequest(const BadRequest& other)
    : HttpException(other) {
    if (this == &other) {
        return;
    }
}

BadRequest::~BadRequest() throw() {
}

IncompleteRequest::IncompleteRequest(string message)
    : BadRequest(message) {
}

IncompleteRequest::IncompleteRequest(const IncompleteRequest& other)
    : BadRequest(other) {
    if (this == &other) {
        return;
    }
}

IncompleteRequest::~IncompleteRequest() throw() {
}

PayloadTooLarge::PayloadTooLarge(string message)
    : BadRequest(message) {
}

PayloadTooLarge::PayloadTooLarge(const PayloadTooLarge& other)
    : BadRequest(other) {
    if (this == &other) {
        return;
    }
}

PayloadTooLarge::~PayloadTooLarge() throw() {
}

ShuttingDown::ShuttingDown() {
}

const char* ShuttingDown::what() const throw() {
    return ("Server is shutting down");
}

ShuttingDown::~ShuttingDown() throw() {
}
}  // namespace webserver
