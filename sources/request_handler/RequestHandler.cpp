#include "RequestHandler.hpp"

#include <iostream>
#include <string>

#include "configuration/AppConfig.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "request/Request.hpp"
#include "request_handler/GetHandler.hpp"
#include "response_generator/ResponseGenerator.hpp"
#include "utils/utils.hpp"

using std::string;

#define PRINT_RESPONSES 1

namespace webserver {

string RequestHandler::handleRequest(Request* request, const AppConfig* appConfig) {
    (void)request;
    // NOTE: currently only works with GET requests

    /* NOTE: 1. Request parsing will happen here
    ParsedRequest req = RequestParser::parse(location);
	*/

    // NOTE: 2. Select method handler; currently only works with GET requests
    ResponseData responseData = GetHandler::handleRequest(
        "tests/e2e/1/test.html",
        appConfig->getRoute("/").getFolderConfig()->getRootPath()
    );
    // NOTE: for the future: request->getLocation());

    // NOTE: 3. Pass to response generator
    string response = ResponseGenerator::generateResponse(responseData);

    if (PRINT_RESPONSES) {
        utils::printSeparator();
        std::clog << response << std::endl;
        utils::printSeparator();
    }
    return (response);
}

}  // namespace webserver
