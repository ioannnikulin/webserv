#include <string>

#include "http_methods/HttpMethodType.hpp"
#include "request_handler/RequestHandler.hpp"

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
    static ResponseData handleRequest(std::string requestBody, std::string rootLocation);
    static ResponseData serveFile(const std::string& path, int statusCode);
    static ResponseData serveStatusPage(int statusCode);
};
}  // namespace webserver
