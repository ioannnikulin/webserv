#include "DeleteHandler.hpp"

#include <cstdio>
#include <string>

#include "configuration/RouteConfig.hpp"
#include "file_system/FileSystem.hpp"
#include "http_status/HttpStatus.hpp"
#include "response/Response.hpp"

using std::string;

namespace webserver {

Response DeleteHandler::handleRequest(string target, const RouteConfig& configuration) {
    (void)configuration;
    if (file_system::isDirectory(target.c_str())) {
        return (configuration.getStatusCatalogue().serveStatusPage(HttpStatus::FORBIDDEN));
    }
    if (!file_system::fileExists(target.c_str())) {
        return (configuration.getStatusCatalogue().serveStatusPage(HttpStatus::NOT_FOUND));
    }
    if (std::remove(target.c_str()) != 0) {
        return (configuration.getStatusCatalogue().serveStatusPage(HttpStatus::INTERNAL_SERVER_ERROR
        ));
    }
    return (configuration.getStatusCatalogue().serveStatusPage(HttpStatus::NO_CONTENT));
}

}  // namespace webserver
