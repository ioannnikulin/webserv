#include "ShutdownHandler.hpp"

#include "http_status/ShuttingDown.hpp"

namespace webserver {
void ShutdownHandler::handleRequest() {
    throw(ShuttingDown());
}
}  // namespace webserver
