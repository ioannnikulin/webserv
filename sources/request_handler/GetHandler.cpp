#include "GetHandler.hpp"

#include <iostream>
#include <string>

#include "file_system/FileSystem.hpp"
#include "file_system/MimeTypes.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "http_status/HttpStatus.hpp"

using std::string;

namespace webserver {

ResponseData GetHandler::serveFile(const std::string& path, int statusCode) {
    ResponseData rd;

    rd.statusCode = statusCode;
    rd.body = file_system::readFile(path.c_str());
    // NOTE: replaced with the next line. rd.contentLength = file_system::getFileSize(path.c_str());
    rd.contentLength = rd.body.size();

    std::string ext = file_system::getFileExtension(path);
    rd.contentType = MimeTypes::getMimeType(ext);

    return rd;
}

ResponseData GetHandler::serveStatusPage(int statusCode) {
    std::string path = HttpStatus::getPageFileLocation(statusCode);
    return serveFile(path, statusCode);
}

ResponseData GetHandler::handleRequest(string location, string rootLocation) {
    // NOTE: DL we do nothing with request body for now, later we will parse it

    // NOTE: STEP 1. Resolve the file path (using server config). Manual path input for now.

    // NOTE: STEP 2. Check if it exists & is readable (via stat())
    // NOTE: has to be turned into a general ValidateFile function
    string finalLocation = rootLocation + "/" + location;
    // NOTE: can be added to Logger. std::clog << "Trying to access: " << finalLocation << std::endl;
    if (!file_system::fileExists(finalLocation.c_str())) {
        return (serveStatusPage(HttpStatus::NOT_FOUND));
    }
    return (serveFile(finalLocation, HttpStatus::OK));
}

}  // namespace webserver
