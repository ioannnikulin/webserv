#include "RequestHandler.hpp"

#include <iostream>
#include <string>

#include "configuration/AppConfig.hpp"
#include "request/Request.hpp"
#include "request_handler/GetHandler.hpp"
#include "response/Response.hpp"
#include "utils/utils.hpp"
#include "configuration/Endpoint.hpp" // add this include

using std::string;

#define PRINT_RESPONSES 0

namespace webserver {

string RequestHandler::handleRequest(Request* request, const AppConfig* appConfig) {

    // NOTE: 1. Select method handler; currently only works with GET requests
    const Response response = GetHandler::handleRequest(
        request->getRequestTarget(),
        appConfig->getRoute("/").getFolderConfig()->getRootPath()
    );

    // NOTE: 3. Pass to response serializer
    const std::string resp = response.serialize();
    if (PRINT_RESPONSES) {
        utils::printSeparator();
        std::clog << resp << std::endl;
        utils::printSeparator();
    }
    return (resp);
}



string RequestHandler::handleRequest(Request* request, const AppConfig* appConfig) {
    const std::set<Endpoint> endpoints = appConfig->getEndpoints();
    if (endpoints.empty()) {
        throw std::runtime_error("No endpoints configured");
    }

    const Endpoint& ep = *endpoints.begin();          // TEMP: single server
    const RouteConfig& route = ep.getRoute("/");      // or later: matchRoute(target)

    const FolderConfig* folder = route.getFolderConfig();
    if (!folder) {
        throw std::runtime_error("Route '/' has no folder config");
    }

    const Response response = GetHandler::handleRequest(
        request->getRequestTarget(),
        folder->getRootPath()
    );

    return response.serialize();
}



}  // namespace webserver


