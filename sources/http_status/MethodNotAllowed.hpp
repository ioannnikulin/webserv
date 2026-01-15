#ifndef METHODNOTALLOWED_HPP
#define METHODNOTALLOWED_HPP

#include "http_status/BadRequest.hpp"

namespace webserver {
class MethodNotAllowed : public BadRequest {
private:
    MethodNotAllowed& operator=(const MethodNotAllowed& other);

public:
    explicit MethodNotAllowed(std::string message);
    MethodNotAllowed(const MethodNotAllowed& other);
    virtual ~MethodNotAllowed() throw();
};
}  // namespace webserver

#endif
