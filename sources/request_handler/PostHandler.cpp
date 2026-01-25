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
        return (configuration.getStatusCatalogue().serveStatusPage(HttpStatus::METHOD_NOT_ALLOWED));
    }
    target = target.substr(
        configuration.getPath().length(),
        target.length() - configuration.getPath().length()
    );
    if (target.empty()) {
        // NOTE: cannot create without filename
        return (configuration.getStatusCatalogue().serveStatusPage(HttpStatus::BAD_REQUEST));
    }
    const string targetFilename = target.substr(target.find_last_of('/', string::npos));
    const string targetFolder = configuration.getUploadConfigSection().getUploadRootFolder() +
                                target.substr(0, target.find_last_of('/'));
    if (!file_system::fileExists(targetFolder.c_str())) {
        // NOTE: we don't have to create subfolders
        return (configuration.getStatusCatalogue().serveStatusPage(HttpStatus::BAD_REQUEST));
    }
    target = targetFolder + targetFilename;
    // NOTE: no, it's not the original argument value, it had route removed
    _log.stream(LOG_DEBUG) << "Preresolved path: " << target << "\n";
    if (file_system::isDirectory(target.c_str())) {
        return (configuration.getStatusCatalogue().serveStatusPage(HttpStatus::BAD_REQUEST));
    }
    std::ofstream file(target.c_str(), std::ios::binary);
    file.write(body.data(), static_cast<std::streamsize>(body.size()));
    file.close();
    return (configuration.getStatusCatalogue().serveStatusPage(HttpStatus::CREATED));
}

}  // namespace webserver
