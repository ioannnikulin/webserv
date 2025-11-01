#include "webserv.hpp"

using std::string;

// TODO: issue 8
string generateResponse(const string& request) {
    (void)request;
    int a = 0;
    int b = 3 / a;
    std::cout << b << std::endl;
    return ("HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!");
}
