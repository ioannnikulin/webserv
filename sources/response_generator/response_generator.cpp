#include <string>

#include "webserv.hpp"

using std::string;

// TODO: issue 8
string generateResponse(const string& request) {
    (void)request;
    int *ptr = 0;
    std::cout << ptr << std::endl;
    return ("HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!");
}
