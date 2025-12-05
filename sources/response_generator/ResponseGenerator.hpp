#ifndef RESPONSE_GENERATOR_HPP
#define RESPONSE_GENERATOR_HPP

#include <string>

#include "http_methods/HttpMethodType.hpp"

namespace webserver {

class ResponseGenerator {
private:
    ResponseGenerator();
    ResponseGenerator(const ResponseGenerator& other);
    ResponseGenerator& operator=(const ResponseGenerator& other);
    ~ResponseGenerator();

public:
    static std::string generateResponse(const ResponseData& responseData);
};
}  // namespace webserver
#endif
