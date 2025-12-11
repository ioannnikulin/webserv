#include "RequestHandler.hpp"

#include <iostream>
#include <string>

#include "configuration/AppConfig.hpp"
#include "request/Request.hpp"
#include "request_handler/GetHandler.hpp"
#include "response/Response.hpp"
#include "utils/utils.hpp"

using std::string;

#define PRINT_RESPONSES 0

namespace webserver {

string RequestHandler::handleRequest(Request* request, const AppConfig* appConfig) {
    (void)request;
    // NOTE: currently only works with GET requests

    /* NOTE: 1. Request parsing will happen here
    ParsedRequest req = RequestParser::parse(location);
	*/

    // NOTE: 2. Select method handler; currently only works with GET requests
    const Response response = GetHandler::handleRequest(
        request->getRequestTarget(),
        appConfig->getRoute("/").getFolderConfig()->getRootPath()
    );
    // NOTE: for the future: request->getLocation());

    // NOTE: 3. Pass to response serializer
    const std::string resp = response.serialize();
    if (PRINT_RESPONSES) {
        utils::printSeparator();
        std::clog << resp << std::endl;
        utils::printSeparator();
    }
    return (resp);
}

}  // namespace webserver
