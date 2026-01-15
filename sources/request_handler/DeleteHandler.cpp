#include "DeleteHandler.hpp"

#include <stdexcept>
#include <string>

#include "configuration/RouteConfig.hpp"
#include "response/Response.hpp"

using std::string;

namespace webserver {

Response DeleteHandler::handleRequest(string target, const RouteConfig& configuration) {
    (void)target;
    (void)configuration;
    throw std::runtime_error("DeleteHandler::handleRequest() not implemented yet");
}

}  // namespace webserver
