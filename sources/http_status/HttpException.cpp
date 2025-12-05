#include "http_status/HttpException.hpp"

#include <sstream>

#include "http_status/BadRequest.hpp"
#include "http_status/HttpStatus.hpp"
#include "http_status/ShuttingDown.hpp"

using std::ostringstream;
using std::string;

namespace webserver {
const char* HttpException::what() const throw() {
    ostringstream oss;
    oss << HttpStatus::getReasonPhrase(_code) << ": " << _message;
    return (oss.str().c_str());
}

HttpStatus::CODE HttpException::getCode() const {
    return (_code);
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

ShuttingDown::ShuttingDown() {
}

const char* ShuttingDown::what() const throw() {
    return ("Server is shutting down");
}

ShuttingDown::~ShuttingDown() throw() {
}
}  // namespace webserver
