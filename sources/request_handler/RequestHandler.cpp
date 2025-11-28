#include "RequestHandler.hpp"

#include <string>

#include "GetHandler.hpp"

using std::string;

namespace webserver {
string RequestHandler::handle(string type, string location) {
    (void)type;
    return (GetHandler::handle(location));
}
}  // namespace webserver
