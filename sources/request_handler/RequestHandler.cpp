#include "RequestHandler.hpp"

#include <string>

#include "GetHandler.hpp"

using std::string;

namespace webserver {
string RequestHandler::handle(string requestBody) {
    // stub. this should parse request header and delegate the body to respective handler.
    // considering all requests to be get for now.
    return (GetHandler::handle(requestBody));
}
}  // namespace webserver
