#include "PostHandler.hpp"

#include <fstream>
#include <iostream>
#include <limits>
#include <string>

#include "configuration/RouteConfig.hpp"
#include "file_system/FileSystem.hpp"
#include "http_status/HttpStatus.hpp"
#include "response/Response.hpp"

using std::clog;
using std::endl;
using std::string;

namespace webserver {

Response PostHandler::handleRequest(string target, string body, const RouteConfig& configuration) {
    if (!configuration.getUploadConfig().isUploadEnabled()) {
        return (file_system::serveStatusPage(HttpStatus::METHOD_NOT_ALLOWED));
    }
    target = target.substr(
        configuration.getPath().length(),
        target.length() - configuration.getPath().length()
    );
    if (target.find_last_of('/') != 0) {
        // NOTE: we don't have to create subfolders
        return (file_system::serveStatusPage(HttpStatus::BAD_REQUEST));
    }
    target = configuration.getUploadConfig().getUploadRootFolder() + target;
    clog << "Preresolved path: " << target << endl;
    if (file_system::isDirectory(target.c_str())) {
        return (file_system::serveStatusPage(HttpStatus::BAD_REQUEST));
    }
    std::ofstream file(target.c_str(), std::ios::binary);
    if (body.size() >
        static_cast<std::string::size_type>(std::numeric_limits<std::streamsize>::max())) {
        return (file_system::serveStatusPage(HttpStatus::PAYLOAD_TOO_LARGE));
    }
    file.write(body.data(), static_cast<std::streamsize>(body.size()));
    file.close();
    return (file_system::serveFile(target, HttpStatus::CREATED));
}

}  // namespace webserver
