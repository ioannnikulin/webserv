#ifndef GETHANDLER_HPP
#define GETHANDLER_HPP

#include <string>

#include "configuration/RouteConfig.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "logger/Logger.hpp"
#include "request_handler/RequestHandler.hpp"
#include "response/Response.hpp"

namespace webserver {
/* NOTE:
A non-instantiable utility class providing only static functions.
Responsible for handling GET HTTP requests by locating
the requested resource, applying server/location rules, optionally executing CGI,
and returning the complete response for serialization and sending back

WARNING:
you will NOT be able to debug this part in gdb, it's callled from a forked process.

*/
class GetHandler {
private:
    static Logger _log;
    GetHandler();
    GetHandler(const GetHandler& other);
    GetHandler& operator=(const GetHandler& other);
    ~GetHandler();
    static Response listDirectory(std::string originalTarget, std::string resolvedTarget);

public:
    static Response handleRequest(
        std::string originalTarget,
        std::string resolvedTarget,
        const RouteConfig& routeConfig
    );
};
}  // namespace webserver
#endif
