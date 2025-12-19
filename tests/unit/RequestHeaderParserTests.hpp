#ifndef GETREQUESTPARSERTESTS_HPP
#define GETREQUESTPARSERTESTS_HPP

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "WebServer.hpp"
#include "http_status/BadRequest.hpp"

using std::clog;
using std::endl;
using std::ofstream;
using std::set;
using std::string;
using webserver::Request;

class RequestHeaderParserTests : public CxxTest::TestSuite {
public:
    void testCurl0() {
        const string raw =
            "GET / HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Accept: */*\r\n\r\n";
        Request expected;
        expected.setType("GET")
            .setRequestTarget("/")
            .setVersion("HTTP/1.1")
            // spaces after colon skipped, spaces after the value are preserved
            .addHeader("Host", "127.10.0.1:8888 ")
            .addHeader("User-Agent", "curl/8.5.0")
            .addHeader("Accept", "*/*");

        TS_SKIP("not implemented");
        Request actual(raw);
        clog << actual.getRequestTarget() << endl;
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testBadLineEndings() {
        const string raw =
            "GET / HTTP/1.1\nHost: 127.10.0.1:8888\nUser-Agent: curl/8.5.0\nAccept: */*\n\n";
        TS_ASSERT_THROWS(Request actual(raw), webserver::BadRequest);
    }

    void testFirstLineTooShort() {
        const string raw =
            "GET /\r\nHost: 127.10.0.1:8888\r\nUser-Agent: curl/8.5.0\r\nAccept: */*\r\n\r\n";
        TS_ASSERT_THROWS(Request actual(raw), webserver::BadRequest);
    }

    void testMissedColon() {
        const string raw =
            "GET / HTTP/1.1\r\nHost: 127.10.0.1:8888\r\nUser-Agent curl/8.5.0\r\nAccept: "
            "*/*\r\n\r\n";
        TS_ASSERT_THROWS(Request actual(raw), webserver::BadRequest);
    }
};
#endif
