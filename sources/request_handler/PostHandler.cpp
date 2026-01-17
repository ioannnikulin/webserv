#include "PostHandler.hpp"

#include <fstream>
#include <iostream>
#include <string>

#include "configuration/RouteConfig.hpp"
#include "file_system/FileSystem.hpp"
#include "http_status/HttpStatus.hpp"
#include "logger/Logger.hpp"
#include "response/Response.hpp"

using std::string;

namespace webserver {
Logger PostHandler::_log;

Response PostHandler::handleRequest(string target, string body, const RouteConfig& configuration) {
    if (!configuration.getUploadConfigSection().isUploadEnabled()) {
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
    target = configuration.getUploadConfigSection().getUploadRootFolder() + target;
    _log.stream(LOG_DEBUG) << "Preresolved path: " << target << "\n";
    if (file_system::isDirectory(target.c_str())) {
        return (file_system::serveStatusPage(HttpStatus::BAD_REQUEST));
    }
    std::ofstream file(target.c_str(), std::ios::binary);
    file.write(body.data(), static_cast<std::streamsize>(body.size()));
    file.close();
    return (file_system::serveStatusPage(HttpStatus::CREATED));
}

}  // namespace webserver
