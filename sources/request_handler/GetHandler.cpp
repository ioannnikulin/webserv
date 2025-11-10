#include "GetHandler.hpp"

#include <string>
using std::string;
namespace webserver {
string GetHandler::handle(string requestBody) {
    (void)requestBody;
    return ("HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!");
}
}  // namespace webserver