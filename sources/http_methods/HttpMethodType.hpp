#ifndef HTTPMETHODTYPE_HPP
#define HTTPMETHODTYPE_HPP

#include <map>
#include <string>

namespace webserver {
enum HttpMethodType { GET, POST, DELETE, SHUTDOWN };

/* NOTE:
Struct to store the HTTP response data, later used by response generator.
*/
struct ResponseData {
    int statusCode;
    long contentLength;
    std::string body;
    std::string contentType;
};
}  // namespace webserver
#endif
