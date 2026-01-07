#include "GetHandler.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "configuration/Endpoint.hpp"
#include "configuration/RouteConfig.hpp"
#include "file_system/FileSystem.hpp"
#include "file_system/MimeType.hpp"
#include "http_status/HttpStatus.hpp"
#include "response/Response.hpp"

using std::clog;
using std::endl;
using std::ostringstream;
using std::string;

namespace webserver {

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
    clog << "Preresolved path: " << oss.str() << endl;
    if (file_system::isDirectory(oss.str().c_str())) {
        clog << "Target is a directory." << endl;
        if (routeConfig.getFolderConfig()->isListingEnabled() &&
            !routeConfig.getFolderConfig()->getIndexPageFilename().empty()) {
            oss << "/" << routeConfig.getFolderConfig()->getIndexPageFilename();
        } else {
            return (serveStatusPage(HttpStatus::FORBIDDEN));
        }
    } else if (file_system::isFile(oss.str().c_str())) {
        clog << "Target is a file." << endl;
        // NOTE: file exists as is
    } else {
        return (serveStatusPage(HttpStatus::NOT_FOUND));
    }
    target = oss.str();
    clog << "GET " << target << endl;
    if (file_system::fileExists(target.c_str())) {
        return (serveFile(target, HttpStatus::OK));
    }
    return (serveStatusPage(HttpStatus::NOT_FOUND));
}

}  // namespace webserver
