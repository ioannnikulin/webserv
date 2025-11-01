#include <cassert>
#include <string>

#include "tests.hpp"

using std::string;

int runUnitTests() {
    const string request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
    const string expectedResponse = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
    const string actualResponse = generateResponse(request);

    assert(actualResponse == expectedResponse);
    return (0);
}
