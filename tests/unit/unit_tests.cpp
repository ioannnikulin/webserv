#include "tests.hpp"

using std::string;

int unit_tests() {
    string request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    string expected_response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
    string actual_response = generate_response(request);

    assert(expected_response == actual_response);
    return (0);
}
