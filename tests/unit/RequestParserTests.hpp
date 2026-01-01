#ifndef GETREQUESTPARSERTESTS_HPP
#define GETREQUESTPARSERTESTS_HPP

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>

#include "WebServer.hpp"
#include "http_status/BadRequest.hpp"
#include "http_status/IncompleteRequest.hpp"

using std::string;
using webserver::Request;

class RequestParserTests : public CxxTest::TestSuite {
public:
    void testCurlGet() {
        const string raw =
            "GET / HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Accept: */*\r\n\r\n";
        Request expected;
        expected.setType(webserver::GET)
            .setRequestTarget("/")
            .setVersion("HTTP/1.1")
            // spaces after colon skipped, spaces after the value are preserved
            .addHeader("Host", "127.10.0.1:8888 ")
            .addHeader("User-Agent", "curl/8.5.0")
            .addHeader("Accept", "*/*");
        Request actual(raw);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlGetBodyIgnored() {
        const string raw =
            "GET / HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Accept: */*\r\nContent-Length: 42\r\n\r\nwrong size, who cares";
        Request expected;
        expected.setType(webserver::GET)
            .setRequestTarget("/")
            .setVersion("HTTP/1.1")
            .addHeader("Host", "127.10.0.1:8888 ")
            .addHeader("User-Agent", "curl/8.5.0")
            .addHeader("Accept", "*/*")
            .addHeader("Content-Length", "42");
        Request actual(raw);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlPost() {
        const string raw =
            "POST /submit HTTP/1.1\r\nHost: 127.10.0.1:8888\r\nUser-Agent: curl/8.5.0\r\nAccept: "
            "*/*\r\nContent-Length: 12\r\n\r\nHello World!";
        Request expected;
        expected.setType(webserver::POST)
            .setRequestTarget("/submit")
            .setVersion("HTTP/1.1")
            .addHeader("Host", "127.10.0.1:8888")
            .addHeader("User-Agent", "curl/8.5.0")
            .addHeader("Accept", "*/*")
            .addHeader("Content-Length", "12")
            .setBody("Hello World!");
        Request actual(raw);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testIncompleteBody() {
        const string raw =
            "POST /submit HTTP/1.1\r\nHost: 127.10.0.1:8888\r\nUser-Agent: curl/8.5.0\r\nAccept: "
            "*/*\r\nContent-Length: 12\r\n\r\nHello Wo";
        TS_ASSERT_THROWS(Request actual(raw), webserver::IncompleteRequest);
    }

    void testExcessiveBody() {
        const string raw =
            "POST /submit HTTP/1.1\r\nHost: 127.10.0.1:8888\r\nUser-Agent: curl/8.5.0\r\nAccept: "
            "*/*\r\nContent-Length: 1\r\n\r\nHello World!";
        TS_ASSERT_THROWS(Request actual(raw), webserver::BadRequest);
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
