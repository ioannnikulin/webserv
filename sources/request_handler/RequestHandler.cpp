#include "RequestHandler.hpp"

#include <string>

#include "configuration/RouteConfig.hpp"
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

using std::string;

namespace webserver {
Logger RequestHandler::_log;

string RequestHandler::serializeAndPrint(const Response& response) {
    const std::string resp = response.serialize();
    if (MimeType::isPrintable(response.getHeader("Content-Type"))) {
        _log.stream(LOG_TRACE) << "Response:\n" << resp << "\n";
    }
    return (resp);
}

string RequestHandler::handleRequest(Request& request, const RouteConfig& configuration) {
    if (request.getType() == SHUTDOWN) {
        const Response resp = Response(
            HttpStatus::HTTP_SERVICE_UNAVAILABLE,
            configuration.getStatusCatalogue().getReasonPhrase(HttpStatus::HTTP_SERVICE_UNAVAILABLE
            ),
            "Server is shutting down",
            MimeType::getMimeType("txt")
        );
        return (serializeAndPrint(resp));
    }
    if (configuration.isRedirection()) {
        // NOTE: yes, redirects are checked before allowed methods
        return (serializeAndPrint(
            Response(
                HttpStatus::MOVED_PERMANENTLY,
                configuration.getStatusCatalogue().getReasonPhrase(HttpStatus::MOVED_PERMANENTLY),
                configuration.getStatusCatalogue().getReasonPhrase(HttpStatus::MOVED_PERMANENTLY),
                MimeType::getMimeType("txt")
            )
                .setHeader("Location", configuration.getRedirection())
        ));
    }
    if (!configuration.isMethodAllowed(request.getType())) {
        return (serializeAndPrint(
            configuration.getStatusCatalogue().serveStatusPage(HttpStatus::METHOD_NOT_ALLOWED)
        ));
    }
    string body;
    request.setMaxClientBodySizeBytes(configuration.getFolderConfig().getMaxClientBodySizeBytes());
    try {
        body = request.getBody();
    } catch (const HttpException& e) {
        // NOTE: BadRequest, PayloadTooLarge
        return (serializeAndPrint(configuration.getStatusCatalogue().serveStatusPage(e.getCode())));
    }
    const string resolvedTarget =
        configuration.getFolderConfig().getResolvedPath(request.getPath());
    Response response(-1, "", "", "");
    switch (request.getType()) {
        case GET: {
            _log.stream(LOG_TRACE) << "Preresolved path: " << resolvedTarget << "\n";
            response = GetHandler::handleRequest(
                request.getPath(),
                resolvedTarget,
                request.isCgiRequest(),
                configuration
            );
            break;
        }
        case POST: {
            // NOTE: path will be reresolved later inside
            if (!request.isCgiRequest()) {
                response = PostHandler::handleRequest(request.getPath(), body, configuration);
            }
            break;
        }
        case DELETE: {
            if (!request.isCgiRequest()) {
                _log.stream(LOG_TRACE) << "Preresolved path: " << resolvedTarget << "\n";
            }
            response = DeleteHandler::handleRequest(resolvedTarget, configuration);
            break;
        }
        default: {
            response =
                configuration.getStatusCatalogue().serveStatusPage(HttpStatus::NOT_IMPLEMENTED);
            break;
        }
    }
    if (response.getStatus() == -1) {
        return ("CGI");
    }
    return (serializeAndPrint(response));
}

}  // namespace webserver
