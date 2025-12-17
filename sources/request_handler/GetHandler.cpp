#include "GetHandler.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include "configuration/AppConfig.hpp"
#include "file_system/FileSystem.hpp"
#include "file_system/MimeTypes.hpp"
#include "http_status/HttpStatus.hpp"
#include "response/Response.hpp"

using std::clog;
using std::endl;
using std::ostringstream;
using std::string;

namespace webserver {

Response GetHandler::serveFile(const std::string& path, int statusCode) {
    Response res;

    res.setStatus(statusCode);
    res.setBody(file_system::readFile(path.c_str()));

    const std::string ext = file_system::getFileExtension(path);
    res.setHeader("Content-Type", webserver::MimeTypes::getMimeType(ext));

    return res;
}

Response GetHandler::serveStatusPage(int statusCode) {
    const std::string path = HttpStatus::getPageFileLocation(statusCode);
    return serveFile(path, statusCode);
}

Response GetHandler::handleRequest(string location, const AppConfig* appConfig) {
    ostringstream oss;
    oss << appConfig->getEndpoints().begin()->getRoute("/").getFolderConfig()->getRootPath() << "/";
    if (location == "/") {
        oss << appConfig->getEndpoints()
                   .begin()
                   ->getRoute("/")
                   .getFolderConfig()
                   ->getIndexPageFileLocation();
    } else {
        if (!location.empty() && location[0] == '/') {
            location = location.substr(1);
        }
        oss << location;
    }
    location = oss.str();
    clog << "GET " << location << endl;
    if (file_system::fileExists(location.c_str())) {
        return (serveFile(location, HttpStatus::OK));
    }
    return (serveStatusPage(HttpStatus::NOT_FOUND));
}
}  // namespace webserver
