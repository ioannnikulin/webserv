#ifndef DELETEHANDLER_HPP
#define DELETEHANDLER_HPP

#include <string>

#include "configuration/RouteConfig.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "request_handler/RequestHandler.hpp"
#include "response/Response.hpp"

namespace webserver {
class DeleteHandler {
private:
    DeleteHandler();
    DeleteHandler(const DeleteHandler& other);
    DeleteHandler& operator=(const DeleteHandler& other);
    ~DeleteHandler();

public:
    static Response handleRequest(std::string target, const RouteConfig& configuration);
};
}  // namespace webserver
#endif
