#include "GetHandler.hpp"

#include <iostream>
#include <string>

#include "file_system/FileSystem.hpp"
#include "file_system/MimeTypes.hpp"
#include "http_status/HttpStatus.hpp"
#include "response/Response.hpp"

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

Response GetHandler::handleRequest(string requestTarget, string rootLocation) {
    // NOTE: STEP 1. Resolve the file path (using server config).

    std::cout << "requestTarget: " << requestTarget << std::endl;
    // NOTE: can be added to Logger. std::clog << "Trying to access: " << finalLocation << std::endl;
    if (requestTarget == "/") {
        requestTarget = "/tests/e2e/1/index.html";
    }
    const string finalLocation = rootLocation + "/" + requestTarget;
    std::cout << "finalLocation: " << finalLocation << std::endl;

    // NOTE: STEP 2. Check if it exists & is readable
    // NOTE: has to be turned into a general ValidateFile function
    if (file_system::fileExists(finalLocation.c_str())) {
        return (serveFile(finalLocation, HttpStatus::OK));
    }
    return (serveStatusPage(HttpStatus::NOT_FOUND));
}
}  // namespace webserver
