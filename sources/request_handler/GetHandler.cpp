#include "GetHandler.hpp"

#include <sstream>
#include <string>

#include "configuration/RouteConfig.hpp"
#include "file_system/FileSystem.hpp"
#include "http_status/HttpStatus.hpp"
#include "logger/Logger.hpp"
#include "response/Response.hpp"

using std::ostringstream;
using std::string;

namespace webserver {
Logger GetHandler::_log;

Response GetHandler::handleRequest(string target, const RouteConfig& routeConfig) {
    if (file_system::isDirectory(target.c_str())) {
        _log.stream(LOG_TRACE) << "Target is a directory.\n";
        if (routeConfig.getFolderConfig().isListingEnabled() &&
            !routeConfig.getFolderConfig().getIndexPageFilename().empty()) {
            target += (target.at(target.size() - 1) == '/' ? "" : "/") +
                      routeConfig.getFolderConfig().getIndexPageFilename();
        } else {
            return (file_system::serveStatusPage(HttpStatus::FORBIDDEN));
        }
    } else if (file_system::isFile(target.c_str())) {
        _log.stream(LOG_DEBUG) << "Target is a file.\n";
        // NOTE: file exists as is
    } else {
        return (file_system::serveStatusPage(HttpStatus::NOT_FOUND));
    }

    _log.stream(LOG_TRACE) << "GET " << target << "\n";

    if (target.find("..") != std::string::npos) {
        _log.stream(LOG_WARN) << "Directory traversal attempt: " << target << "\n";
        return (file_system::serveStatusPage(HttpStatus::FORBIDDEN));
    }

    if (file_system::fileExists(target.c_str())) {
        return (file_system::serveFile(target, HttpStatus::OK));
    }

    return (file_system::serveStatusPage(HttpStatus::NOT_FOUND));
}

}  // namespace webserver
