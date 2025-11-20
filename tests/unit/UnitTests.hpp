#ifndef UNITTESTS_HPP
#define UNITTESTS_HPP

#include <cxxtest/TestSuite.h>

#include <string>

#include "WebServer.hpp"

using std::string;

class ResponseGeneratorTest : public CxxTest::TestSuite {
public:
    void testResponseGenerator() {
        const string request = "GET / HTTP/1.1\r\nHost: localhost\r\n\r\n";
        const string expectedResponse =
            "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
        const string actualResponse = webserver::generateResponse(request);
        TS_ASSERT_EQUALS(expectedResponse, actualResponse);
    }
};
#endif
