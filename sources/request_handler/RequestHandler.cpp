#include "RequestHandler.hpp"

#include <iostream>
#include <string>

#include "configuration/Endpoint.hpp"
#include "request/Request.hpp"
#include "request_handler/GetHandler.hpp"
#include "response/Response.hpp"
#include "utils/colors.hpp"
#include "utils/utils.hpp"

using std::string;

#define PRINT_RESPONSES 0

namespace webserver {

string RequestHandler::handleRequest(const Request& request, const Endpoint& configuration) {
    // NOTE: 1. Select method handler; currently only works with GET requests
    const Response response = GetHandler::handleRequest(request.getRequestTarget(), configuration);
    // NOTE: 2. After the response is formed, pass to response serializer
    const std::string resp = response.serialize();
    if (PRINT_RESPONSES) {
        std::clog << utils::separator() << GREY << resp << RESET_COLOR << utils::separator()
                  << std::endl;
    }
    return (resp);
}

}  // namespace webserver
