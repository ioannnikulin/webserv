#include "RequestHandler.hpp"

#include <string>

#include "configuration/RouteConfig.hpp"
#include "file_system/FileSystem.hpp"
#include "file_system/MimeType.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "http_status/HttpException.hpp"
#include "http_status/HttpStatus.hpp"
#include "logger/Logger.hpp"
#include "request/Request.hpp"
#include "request_handler/DeleteHandler.hpp"
#include "request_handler/GetHandler.hpp"
#include "request_handler/PostHandler.hpp"
#include "response/Response.hpp"
#include "utils/colors.hpp"
#include "utils/utils.hpp"

using std::string;

namespace webserver {
Logger RequestHandler::_log;

string RequestHandler::serializeAndPrint(const Response& response) {
    const std::string resp = response.serialize();
    if (MimeType::isPrintable(response.getHeader("Content-Type"))) {
        _log.stream(LOG_TRACE) << utils::separator() << "Response:\n"
                               << GREY << resp << RESET_COLOR << utils::separator() << "\n";
    }
    return (resp);
}

string RequestHandler::handleRequest(Request& request, const RouteConfig& configuration) {
    if (request.getType() != SHUTDOWN && !configuration.isMethodAllowed(request.getType())) {
        return (serializeAndPrint(file_system::serveStatusPage(HttpStatus::METHOD_NOT_ALLOWED)));
    }
    string body;
    request.setMaxClientBodySizeBytes(configuration.getFolderConfig().getMaxClientBodySizeBytes());
    try {
        body = request.getBody();
    } catch (const HttpException& e) {
        // NOTE: BadRequest, PayloadTooLarge
        return (serializeAndPrint(file_system::serveStatusPage(e.getCode())));
    }
    const string resolvedTarget =
        configuration.getFolderConfig().getResolvedPath(request.getPath());
    Response response;
    switch (request.getType()) {
        case GET: {
            _log.stream(LOG_TRACE) << "Preresolved path: " << resolvedTarget << "\n";
            response = GetHandler::handleRequest(resolvedTarget, configuration);
            break;
        }
        case POST: {
            // NOTE: path will be reresolved later inside
            response = PostHandler::handleRequest(request.getPath(), body, configuration);
            break;
        }
        case DELETE: {
            _log.stream(LOG_TRACE) << "Preresolved path: " << resolvedTarget << "\n";
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
