#include "ResponseGenerator.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include "http_status/HttpStatus.hpp"
#include "utils/colors.hpp"
#include "utils/utils.hpp"

using std::string;

#define HTTP_PROTOCOL "HTTP/1.0"

namespace webserver {

// TODO 58:
string ResponseGenerator::generateResponse(const ResponseData& responseData) {
    std::clog << B_YELLOW << "Generating the HTTP response..." << RESET << std::endl;

    std::ostringstream resp;

    resp << HTTP_PROTOCOL << " " << responseData.statusCode << " ";
    resp << HttpStatus::getReasonPhrase(responseData.statusCode);
    resp << "\r\n";
    resp << "Content-Length: " << responseData.contentLength;
    resp << "\r\n";
    resp << "Content-Type: " << responseData.contentType;
    resp << "\r\n\r\n";
    resp << responseData.body;

    std::clog << B_GREEN << "✅ HTTP response generated!" << RESET << std::endl;

    return (resp.str());
}
}  // namespace webserver
