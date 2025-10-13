#include "webserv.hpp"

using std::string;

string generate_response(const string &request) {
    (void)request;
    return ("HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!");
}
