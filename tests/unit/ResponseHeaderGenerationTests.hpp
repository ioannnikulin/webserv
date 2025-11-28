#ifndef RESPONSEHEADERGENERATIONTESTS_HPP
#define RESPONSEHEADERGENERATIONTESTS_HPP

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "WebServer.hpp"

using std::endl;
using std::ofstream;
using std::set;
using std::string;
using webserver::Request;

class ResponseHeadersGenerationTests : public CxxTest::TestSuite {
public:
    void testResponse0() {
        string body = "Hello world!";
        string expected = "HTTP/1.0 200 OK\r\nContent-Length: 13\r\n\r\nHello world!";

        TS_SKIP("response header generation not implemented");

        webserver::Connection c;
        c.setResponseBuffer(body);
        TS_ASSERT_THROWS_NOTHING(c.generateResponseHeaders());
        TS_ASSERT_EQUALS(expected, c.getResponseBuffer());
    }
};
#endif
