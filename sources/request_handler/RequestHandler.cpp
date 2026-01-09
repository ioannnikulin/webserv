#include "RequestHandler.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

#include "configuration/Endpoint.hpp"
#include "configuration/RouteConfig.hpp"
#include "file_system/FileSystem.hpp"
#include "file_system/MimeType.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "http_status/HttpStatus.hpp"
#include "request/Request.hpp"
#include "request_handler/DeleteHandler.hpp"
#include "request_handler/GetHandler.hpp"
#include "request_handler/PostHandler.hpp"
#include "response/Response.hpp"
#include "utils/colors.hpp"
#include "utils/utils.hpp"

using std::clog;
using std::endl;
using std::string;

#define PRINT_RESPONSES 1

namespace webserver {
string serializeAndPrint(const Response& response) {
    const std::string resp = response.serialize();
    if ((PRINT_RESPONSES == 1) && MimeType::isPrintable(response.getHeader("Content-Type"))) {
        std::clog << utils::separator() << "Response:\n"
                  << GREY << resp << RESET_COLOR << utils::separator() << std::endl;
    }
    return (resp);
}

string RequestHandler::handleRequest(Request& request, const Endpoint& configuration) {
    string target = request.getRequestTarget();
    if (target.find('?') != string::npos) {
        // NOTE: ignoring queries for now
        target = target.substr(0, target.find('?'));
    }
    RouteConfig routeConfig;
    try {
        routeConfig = configuration.selectRoute(target);
    } catch (const std::out_of_range& e) {
        return (serializeAndPrint(file_system::serveStatusPage(HttpStatus::NOT_FOUND)));
    }
    string body;
    request.setMaxBodySizeBytes(routeConfig.getFolderConfig().getMaxClientBodySizeBytes());
    try {
        body = request.getBody();
    } catch (const std::exception& e) {
        return (serializeAndPrint(file_system::serveStatusPage(HttpStatus::BAD_REQUEST)));
    }
    const string resolvedTarget = routeConfig.getFolderConfig().getResolvedPath(target);
    if (request.getType() != SHUTDOWN && !routeConfig.isMethodAllowed(request.getType())) {
        return (serializeAndPrint(file_system::serveStatusPage(HttpStatus::METHOD_NOT_ALLOWED)));
    }
    Response response;
    switch (request.getType()) {
        case GET: {
            clog << "Preresolved path: " << resolvedTarget << endl;
            response = GetHandler::handleRequest(resolvedTarget, routeConfig);
            break;
        }
        case POST: {
            response = PostHandler::handleRequest(target, body, routeConfig);
            break;
        }
        case DELETE: {
            clog << "Preresolved path: " << resolvedTarget << endl;
            response = DeleteHandler::handleRequest(resolvedTarget, configuration);
            break;
        }
        case SHUTDOWN: {
            response = Response(
                HttpStatus::HTTP_SERVICE_UNAVAILABLE,
                "Server is shutting down",
                "text/html"
            );
            response.setHeader("Connection", "close");
            break;
        }
        default: {
            response = file_system::serveStatusPage(HttpStatus::NOT_IMPLEMENTED);
            break;
        }
    }
    return (serializeAndPrint(response));
}

}  // namespace webserver
