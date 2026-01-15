#ifndef REQUESTHANDLER_HPP
#define REQUESTHANDLER_HPP

#include <string>

#include "configuration/AppConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "logger/Logger.hpp"
#include "request/Request.hpp"
#include "response/Response.hpp"

namespace webserver {
/* NOTE:
Parsing raw HTTP request and selecting a responsible handler.

WARNING:
you will NOT be able to debug this part in gdb, it's callled from a forked process.

*/
class RequestHandler {
private:
    static Logger _log;
    RequestHandler();
    RequestHandler(const RequestHandler& other);
    RequestHandler& operator=(const RequestHandler& other);
    static std::string serializeAndPrint(const Response& response);

public:
    ~RequestHandler();
    static std::string handleRequest(Request& request, const RouteConfig& configuration);
};

}  // namespace webserver
#endif
