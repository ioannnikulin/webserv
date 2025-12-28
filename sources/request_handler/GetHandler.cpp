#include "GetHandler.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include "configuration/Endpoint.hpp"
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
    const std::string ext = file_system::getFileExtension(path);
    const Response resp(
        statusCode,
        file_system::readFile(path.c_str()),
        MimeTypes::getMimeType(ext)
    );
    return (resp);
}

Response GetHandler::serveStatusPage(int statusCode) {
    const std::string path = HttpStatus::getPageFileLocation(statusCode);
    return serveFile(path, statusCode);
}

Response GetHandler::handleRequest(string location, const Endpoint& configuration) {
    ostringstream oss;
    oss << configuration.getRoute("/").getFolderConfig()->getRootPath() << "/";
    if (location == "/") {
        oss << configuration.getRoute("/").getFolderConfig()->getIndexPageFileLocation();
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
