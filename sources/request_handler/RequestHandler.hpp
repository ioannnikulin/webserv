#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <string>

#include "configuration/AppConfig.hpp"
#include "request/Request.hpp"

namespace webserver {
/* NOTE:
Parsing raw HTTP request and selecting a responsible handler.
*/
class RequestHandler {
private:
    RequestHandler();
    RequestHandler(const RequestHandler& other);
    RequestHandler& operator=(const RequestHandler& other);

public:
    ~RequestHandler();
    static std::string handleRequest(Request* request, const AppConfig* appConfig);
};

}  // namespace webserver
#endif
