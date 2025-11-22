#include "RequestHandler.hpp"

#include <string>

#include "GetHandler.hpp"

using std::string;

namespace webserver {
string RequestHandler::handle(string requestBody) {
    return (GetHandler::handle(requestBody));
}
}  // namespace webserver
