#ifndef BADREQUEST_HPP
#define BADREQUEST_HPP

#include "http_status/HttpException.hpp"

namespace webserver {
class BadRequest : public HttpException {
private:
    BadRequest& operator=(const BadRequest& other);

public:
    explicit BadRequest(std::string message);
    BadRequest(const BadRequest& other);
    virtual ~BadRequest() throw();
};
}  // namespace webserver

#endif
