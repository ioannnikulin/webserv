#ifndef NOTFOUND_HPP
#define NOTFOUND_HPP

#include "http_status/HttpException.hpp"

namespace webserver {
class NotFound : public HttpException {
private:
    NotFound& operator=(const NotFound& other);

public:
    explicit NotFound(std::string message);
    NotFound(const NotFound& other);
    virtual ~NotFound() throw();
};
}  // namespace webserver

#endif
