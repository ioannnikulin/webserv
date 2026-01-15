#ifndef POSTHANDLER_HPP
#define POSTHANDLER_HPP

#include <string>

#include "configuration/RouteConfig.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "logger/Logger.hpp"
#include "request_handler/RequestHandler.hpp"
#include "response/Response.hpp"

namespace webserver {
class PostHandler {
private:
    static Logger _log;
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
