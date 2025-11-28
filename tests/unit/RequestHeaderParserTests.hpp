#ifndef GETREQUESTPARSERTESTS_HPP
#define GETREQUESTPARSERTESTS_HPP

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

class RequestHeaderParserTests : public CxxTest::TestSuite {
public:
    void testRequest0() {
        const string raw =
            "GET / HTTP/1.1\nHost: 127.10.0.1:8888\nUser-Agent: curl/8.5.0\nAccept: */*\n\n";
        Request expected;
        expected.setType("GET")
            .setLocation("/")
            .setProtocol("HTTP/1.1")
            .addHeader("Host", "127.10.0.1:8888")
            .addHeader("User-Agent", "curl/8.5.0")
            .addHeader("Accept", "*/*");

        TS_SKIP("header parsing not implemented");

        TS_ASSERT_THROWS_NOTHING(Request(raw));
        Request actual(raw);
        TS_ASSERT_EQUALS(expected, actual);
    }
};
#endif
