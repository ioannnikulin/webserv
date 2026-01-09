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

using std::cout;
using std::endl;
using std::ostringstream;
using std::string;
using webserver::Request;

class RequestParserTests : public CxxTest::TestSuite {
public:
    void testCurlGet() {
        TS_SKIP("debug");
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
        actual.getBody(); // lazy body
        expected.getBody();
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlGetBodyIgnored() {
        TS_SKIP("debug");
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
            .addHeader("Content-Length", "42").setBody("");
        Request actual(raw);
        actual.getBody(); // lazy body
        expected.getBody();
        cout << expected << endl;
        cout << actual << endl;
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlPost() {
        TS_SKIP("debug");
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
        actual.getBody(); // lazy body
        expected.getBody();
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlPostChunked() {
        TS_SKIP("debug");
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
        actual.getBody(); // lazy body
        expected.getBody();
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlPostChunkedEmptyBody() {
        TS_SKIP("debug");
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n0\r\n\r\n";
        Request expected;
        expected.setType(webserver::POST)
            .setRequestTarget("/post")
            .setVersion("HTTP/1.1")
            // spaces after colon skipped, spaces after the value are preserved
            .addHeader("Host", "127.10.0.1:8888 ")
            .addHeader("User-Agent", "curl/8.5.0")
            .addHeader("Transfer-Encoding", "chunked");
        Request actual(raw);
        actual.getBody(); // lazy body
        expected.getBody();
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlPostChunkedNoChunks() {
        TS_SKIP("debug");
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n";
        TS_ASSERT_THROWS(Request act(raw), webserver::IncompleteRequest);
    }

    void testCurlPostChunkedBadChunk1() {
        TS_SKIP("debug");
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\nAA\r\n World!\r\n0\r\n\r\n";
        TS_ASSERT_THROWS(Request act(raw), webserver::IncompleteRequest);
    }

    void testCurlPostChunkedBadChunk2() {
        TS_SKIP("debug");
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n9\r\n World!\r\n0\r\n\r\n";
        TS_ASSERT_THROWS(Request act(raw), webserver::BadRequest);
    }

    void testCurlPostChunkedBadChunk3() {
        TS_SKIP("debug");
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\nA\r\n World!\r\n0\r\n\r\n";
        TS_ASSERT_THROWS(Request act(raw), webserver::BadRequest);
    }

    void testCurlPostChunkedBadChunk4() {
        TS_SKIP("debug");
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n6\r\n World!\r\n0\r\n\r\n";
        TS_ASSERT_THROWS(Request act(raw), webserver::BadRequest);
    }

    void testCurlPostChunkedBodyTooLarge() {
        TS_SKIP("debug");
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
        TS_SKIP("debug");
        ostringstream oss;
        oss << "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            << "Content-Length: 5\r\n\r\nHello";
        Request res(oss.str());
        res.setMaxClientBodySizeBytes(4);

        TS_ASSERT_THROWS(res.getBody(), webserver::PayloadTooLarge);
    }

    void testIncompleteBody() {
        TS_SKIP("debug");
        const string raw =
            "POST /submit HTTP/1.1\r\nHost: 127.10.0.1:8888\r\nUser-Agent: curl/8.5.0\r\nAccept: "
            "*/*\r\nContent-Length: 12\r\n\r\nHello Wo";
        TS_ASSERT_THROWS(Request actual(raw), webserver::IncompleteRequest);
    }

    void testExcessiveBody() {
        TS_SKIP("debug");
        const string raw =
            "POST /submit HTTP/1.1\r\nHost: 127.10.0.1:8888\r\nUser-Agent: curl/8.5.0\r\nAccept: "
            "*/*\r\nContent-Length: 1\r\n\r\nHello World!";
        TS_ASSERT_THROWS(Request actual(raw), webserver::BadRequest);
    }

    void testBadLineEndings() {
        TS_SKIP("debug");
        const string raw =
            "GET / HTTP/1.1\nHost: 127.10.0.1:8888\nUser-Agent: curl/8.5.0\nAccept: */*\n\n";
        TS_ASSERT_THROWS(Request actual(raw), webserver::BadRequest);
    }

    void testFirstLineTooShort() {
        TS_SKIP("debug");
        const string raw =
            "GET /\r\nHost: 127.10.0.1:8888\r\nUser-Agent: curl/8.5.0\r\nAccept: */*\r\n\r\n";
        TS_ASSERT_THROWS(Request actual(raw), webserver::BadRequest);
    }

    void testMissedColon() {
        TS_SKIP("debug");
        const string raw =
            "GET / HTTP/1.1\r\nHost: 127.10.0.1:8888\r\nUser-Agent curl/8.5.0\r\nAccept: "
            "*/*\r\n\r\n";
        TS_ASSERT_THROWS(Request actual(raw), webserver::BadRequest);
    }
};
#endif
