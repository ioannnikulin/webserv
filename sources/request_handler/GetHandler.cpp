#include "GetHandler.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "configuration/RouteConfig.hpp"
#include "file_system/FileSystem.hpp"
#include "http_status/HttpStatus.hpp"
#include "response/Response.hpp"

using std::clog;
using std::endl;
using std::ostringstream;
using std::string;

namespace webserver {

Response GetHandler::handleRequest(string target, const RouteConfig& routeConfig) {
    if (file_system::isDirectory(target.c_str())) {
        clog << "Target is a directory." << endl;
        if (routeConfig.getFolderConfig().isListingEnabled() &&
            !routeConfig.getFolderConfig().getIndexPageFilename().empty()) {
            target += (target.at(target.size() - 1) == '/' ? "" : "/") +
                      routeConfig.getFolderConfig().getIndexPageFilename();
        } else {
            return (file_system::serveStatusPage(HttpStatus::FORBIDDEN));
        }
    } else if (file_system::isFile(target.c_str())) {
        clog << "Target is a file." << endl;
        // NOTE: file exists as is
    } else {
        return (file_system::serveStatusPage(HttpStatus::NOT_FOUND));
    }
    clog << "GET " << target << endl;
    if (file_system::fileExists(target.c_str())) {
        return (file_system::serveFile(target, HttpStatus::OK));
    }
    return (file_system::serveStatusPage(HttpStatus::NOT_FOUND));
}

}  // namespace webserver
