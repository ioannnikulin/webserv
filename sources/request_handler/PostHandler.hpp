#ifndef POSTHANDLER_HPP
#define POSTHANDLER_HPP

#include <string>

#include "http_methods/HttpMethodType.hpp"
#include "request_handler/RequestHandler.hpp"
#include "response/Response.hpp"

namespace webserver {
class PostHandler {
private:
    PostHandler();
    PostHandler(const PostHandler& other);
    PostHandler& operator=(const PostHandler& other);
    ~PostHandler();

public:
    static Response
    handleRequest(std::string target, std::string body, const RouteConfig& configuration);
};
}  // namespace webserver
#endif
