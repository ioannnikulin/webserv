#ifndef REQUESTPARSERTESTS_HPP
#define REQUESTPARSERTESTS_HPP

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "WebServer.hpp"
#include "http_status/BadRequest.hpp"
#include "http_status/IncompleteRequest.hpp"
#include "http_status/PayloadTooLarge.hpp"

using std::ostringstream;
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

    void testCurlPostChunked() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n7\r\n World!\r\n0\r\n\r\n";
        Request expected;
        expected.setType(webserver::POST)
            .setRequestTarget("/post")
            .setVersion("HTTP/1.1")
            // spaces after colon skipped, spaces after the value are preserved
            .addHeader("Host", "127.10.0.1:8888 ")
            .addHeader("User-Agent", "curl/8.5.0")
            .addHeader("Transfer-Encoding", "chunked")
            .setBody("Hello World!");
        Request actual(raw);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlPostChunkedBadChunk1() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\nAA\r\n World!\r\n0\r\n\r\n";
        TS_ASSERT_THROWS(Request act(raw), webserver::IncompleteRequest);
    }

    void testCurlPostChunkedBadChunk2() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n9\r\n World!\r\n0\r\n\r\n";
        TS_ASSERT_THROWS(Request act(raw), webserver::BadRequest);
    }

    void testCurlPostChunkedBadChunk3() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\nA\r\n World!\r\n0\r\n\r\n";
        TS_ASSERT_THROWS(Request act(raw), webserver::BadRequest);
    }

    void testCurlPostChunkedBadChunk4() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n6\r\n World!\r\n0\r\n\r\n";
        TS_ASSERT_THROWS(Request act(raw), webserver::BadRequest);
    }

    void testCurlPostChunkedBodyTooLarge() {
        ostringstream oss;
        oss << "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            << "Transfer-Encoding: chunked\r\n\r\n";
        for (int i = 0; i < 1000; ++i) {
            oss << "5\r\nHello\r\n";
        }
        oss << "0\r\n\r\n";

        TS_ASSERT_THROWS(Request act(oss.str()), webserver::PayloadTooLarge);
    }

    void testCurlPostBodyTooLarge() {
        ostringstream oss;
        oss << "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            << "Content-Length: 5000\r\n\r\n";
        for (int i = 0; i < 1000; ++i) {
            oss << "Hello";
        }

        TS_ASSERT_THROWS(Request act(oss.str()), webserver::PayloadTooLarge);
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
