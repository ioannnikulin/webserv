#include "RequestHandler.hpp"

#include <iostream>
#include <string>

#include "configuration/Endpoint.hpp"
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

using std::string;

#define PRINT_RESPONSES 1

namespace webserver {

string RequestHandler::handleRequest(const Request& request, const Endpoint& configuration) {
    Response response;
    switch (request.getType()) {
        case GET: {
            response = GetHandler::handleRequest(request.getRequestTarget(), configuration);
            break;
        }
        case POST: {
            response = PostHandler::handleRequest(
                request.getRequestTarget(),
                request.getBody(),
                configuration
            );
            break;
        }
        case DELETE: {
            response = DeleteHandler::handleRequest(request.getRequestTarget(), configuration);
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
            response = GetHandler::serveStatusPage(HttpStatus::NOT_IMPLEMENTED);
            break;
        }
    }
    const std::string resp = response.serialize();
    if ((PRINT_RESPONSES == 1) && MimeType::isPrintable(response.getHeader("Content-Type"))) {
        std::clog << utils::separator() << GREY << resp << RESET_COLOR << utils::separator()
                  << std::endl;
    }
    return (resp);
}

}  // namespace webserver
