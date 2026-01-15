#include "DeleteHandler.hpp"

#include <cstdio>
#include <string>

#include "file_system/FileSystem.hpp"
#include "http_status/HttpStatus.hpp"
#include "response/Response.hpp"

using std::string;

namespace webserver {

Response DeleteHandler::handleRequest(string target) {
    if (file_system::isDirectory(target.c_str())) {
        return (file_system::serveStatusPage(HttpStatus::FORBIDDEN));
    }
    if (!file_system::fileExists(target.c_str())) {
        return (file_system::serveStatusPage(HttpStatus::NOT_FOUND));
    }
    if (std::remove(target.c_str()) != 0) {
        return (file_system::serveStatusPage(HttpStatus::INTERNAL_SERVER_ERROR));
    }
    return (file_system::serveStatusPage(HttpStatus::NO_CONTENT));
}

}  // namespace webserver
