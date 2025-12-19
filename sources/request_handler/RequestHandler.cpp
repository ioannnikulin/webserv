#include "RequestHandler.hpp"

#include <iostream>
#include <string>

#include "configuration/AppConfig.hpp"
#include "request/Request.hpp"
#include "request_handler/GetHandler.hpp"
#include "response/Response.hpp"
#include "utils/utils.hpp"

using std::string;

#define PRINT_RESPONSES 1

namespace webserver {

string RequestHandler::handleRequest(Request* request, const AppConfig* appConfig) {
    (void)appConfig;

    // NOTE: 1. Select method handler; currently only works with GET requests
    const Response response = GetHandler::handleRequest(request->getRequestTarget(), appConfig);
    // NOTE: 2. After the response is formed, pass to response serializer
    const std::string resp = response.serialize();
    if (PRINT_RESPONSES) {
        utils::printSeparator();
        std::cout << resp << std::endl;
        utils::printSeparator();
    }
    return (resp);
}

}  // namespace webserver
