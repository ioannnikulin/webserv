#include "tests.hpp"

int runUnitTests() {
    std::string request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    std::string expectedResponse = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
    std::string actualResponse = generateResponse(request);

    assert(actualResponse == expectedResponse);
    return (0);
}
