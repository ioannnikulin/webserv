#ifndef GETHANDLER_HPP
#define GETHANDLER_HPP

#include <string>

#include "http_methods/HttpMethodType.hpp"
#include "request_handler/RequestHandler.hpp"
#include "response/Response.hpp"

namespace webserver {
/* NOTE:
A non-instantiable utility class providing only static functions.
Responsible for handling GET HTTP requests by locating
the requested resource, applying server/location rules, optionally executing CGI,
and returning the resource data (or an error) to be formatted into an HTTP response.
*/
class GetHandler {
private:
    GetHandler();
    GetHandler(const GetHandler& other);
    GetHandler& operator=(const GetHandler& other);
    ~GetHandler();

public:
    static Response handleRequest(std::string location, const Endpoint& configuration);
    static Response serveFile(const std::string& path, int statusCode);
    static Response serveStatusPage(int statusCode);
};
}  // namespace webserver
#endif
