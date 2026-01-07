#include "PostHandler.hpp"

#include <stdexcept>
#include <string>

#include "configuration/Endpoint.hpp"
#include "response/Response.hpp"

using std::string;

namespace webserver {

Response PostHandler::handleRequest(string target, string body, const Endpoint& configuration) {
    (void)target;
    (void)body;
    (void)configuration;
    throw std::runtime_error("PostHandler::handleRequest() not implemented yet");
}

}  // namespace webserver
