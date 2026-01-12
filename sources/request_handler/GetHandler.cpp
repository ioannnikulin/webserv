#include "GetHandler.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

#include "configuration/Endpoint.hpp"
#include "configuration/RouteConfig.hpp"
#include "file_system/FileSystem.hpp"
#include "file_system/MimeType.hpp"
#include "http_status/HttpStatus.hpp"
#include "logger/Logger.hpp"
#include "response/Response.hpp"

using std::ostringstream;
using std::string;

namespace webserver {

Logger GetHandler::_log;

Response GetHandler::serveFile(const std::string& path, int statusCode) {
    const std::string ext = file_system::getFileExtension(path);
    const Response resp(
        statusCode,
        file_system::readFile(path.c_str()),
        MimeType::getMimeType(ext)
    );
    return (resp);
}

Response GetHandler::serveStatusPage(int statusCode) {
    const std::string path = HttpStatus::getPageFileLocation(statusCode);
    return (serveFile(path, statusCode));
}

Response GetHandler::handleRequest(string target, const Endpoint& configuration) {
    if (target.find('?') != string::npos) {
        // NOTE: ignoring queries for now
        target = target.substr(0, target.find('?'));
    }
    RouteConfig routeConfig;
    try {
        routeConfig = configuration.selectRoute(target);
    } catch (const std::out_of_range& e) {
        return (serveStatusPage(HttpStatus::NOT_FOUND));
    }
    ostringstream oss;
    oss << routeConfig.getFolderConfig()->getResolvedPath(target);
    _log.stream(LOG_DEBUG) << "Preresolved path: " << oss.str() << "\n";
    if (file_system::isDirectory(oss.str().c_str())) {
        _log.stream(LOG_DEBUG) << "Target is a directory\n";
        if (routeConfig.getFolderConfig()->isListingEnabled() &&
            !routeConfig.getFolderConfig()->getIndexPageFilename().empty()) {
            oss << "/" << routeConfig.getFolderConfig()->getIndexPageFilename();
        } else {
            return (serveStatusPage(HttpStatus::FORBIDDEN));
        }
    } else if (file_system::isFile(oss.str().c_str())) {
        _log.stream(LOG_DEBUG) << "Target is a file.\n";
        // NOTE: file exists as is
    } else {
        return (serveStatusPage(HttpStatus::NOT_FOUND));
    }

    const std::string filePath = oss.str();

    _log.stream(LOG_TRACE) << "GET " << filePath << "\n";

    if (filePath.find("..") != std::string::npos) {
        _log.stream(LOG_WARN) << "Directory traversal attempt: " << filePath << "\n";
        return (serveStatusPage(HttpStatus::FORBIDDEN));
    }

    if (file_system::fileExists(filePath.c_str())) {
        return (serveFile(filePath, HttpStatus::OK));
    }

    return (serveStatusPage(HttpStatus::NOT_FOUND));
}

}  // namespace webserver
