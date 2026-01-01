#ifndef INCOMPLETEREQUEST_HPP
#define INCOMPLETEREQUEST_HPP

#include "http_status/BadRequest.hpp"

namespace webserver {
class IncompleteRequest : public BadRequest {
private:
    IncompleteRequest& operator=(const IncompleteRequest& other);

public:
    explicit IncompleteRequest(std::string message);
    IncompleteRequest(const IncompleteRequest& other);
    virtual ~IncompleteRequest() throw();
};
}  // namespace webserver

#endif
