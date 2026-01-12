#include "RequestHandler.hpp"

#include <string>

#include "configuration/Endpoint.hpp"
#include "file_system/MimeType.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "http_status/HttpStatus.hpp"
#include "logger/Logger.hpp"
#include "request/Request.hpp"
#include "request_handler/DeleteHandler.hpp"
#include "request_handler/GetHandler.hpp"
#include "request_handler/PostHandler.hpp"
#include "response/Response.hpp"

using std::string;

#define PRINT_RESPONSES 1

namespace webserver {

Logger RequestHandler::_log;

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
        _log.stream(LOG_TRACE) << resp;
    }
    return (resp);
}

}  // namespace webserver
