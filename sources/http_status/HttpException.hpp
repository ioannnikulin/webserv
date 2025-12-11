#ifndef HTTPEXCEPTION_HPP
#define HTTPEXCEPTION_HPP

#include <exception>

#include "http_status/HttpStatus.hpp"

namespace webserver {
class HttpException : public std::exception {
private:
    HttpStatus::CODE _code;
    std::string _message;
    mutable std::string _resultBuffer;

protected:
    HttpException();
    HttpException& operator=(const HttpException& other);

public:
    HttpException(HttpStatus::CODE code, std::string message)
        : _code(code)
        , _message(message) {
    }
    HttpException(const HttpException& other);
    virtual ~HttpException() throw();
    const char* what() const throw();
    HttpStatus::CODE getCode() const;
};
}  // namespace webserver
#endif
