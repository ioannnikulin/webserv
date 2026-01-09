#ifndef PAYLOADTOOLARGE_HPP
#define PAYLOADTOOLARGE_HPP

#include "http_status/BadRequest.hpp"

namespace webserver {
class PayloadTooLarge : public BadRequest {
private:
    PayloadTooLarge& operator=(const PayloadTooLarge& other);

public:
    explicit PayloadTooLarge(std::string message);
    PayloadTooLarge(const PayloadTooLarge& other);
    virtual ~PayloadTooLarge() throw();
};
}  // namespace webserver

#endif
