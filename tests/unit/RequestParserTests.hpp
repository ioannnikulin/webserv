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
#include "logger/LoggerConfig.hpp"

using std::cout;
using std::endl;
using std::ostringstream;
using std::string;
using webserver::Request;

class RequestParserTests : public CxxTest::TestSuite {
public:
    void setUp() {
        webserver::LoggerConfig::setGlobalLevel(LOG_SILENT);
    }

    void testCurlGet() {
        const string raw =
            "GET / HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Accept: */*\r\n\r\n";
        Request expected;
        expected.setType(webserver::GET)
            .setRequestTarget("/")
            .setPath("/")
            .setVersion("HTTP/1.1")
            // spaces after colon skipped, spaces after the value are preserved
            .addHeader("Host", "127.10.0.1:8888 ")
            .addHeader("User-Agent", "curl/8.5.0")
            .addHeader("Accept", "*/*");
        Request actual(raw);
        actual.getBody();  // lazy body
        expected.getBody();
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlGetBodyIgnored() {
        const string raw =
            "GET / HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Accept: */*\r\nContent-Length: 42\r\n\r\nwrong size, who cares";
        Request expected;
        expected.setType(webserver::GET)
            .setRequestTarget("/")
            .setPath("/")
            .setVersion("HTTP/1.1")
            .addHeader("Host", "127.10.0.1:8888 ")
            .addHeader("User-Agent", "curl/8.5.0")
            .addHeader("Accept", "*/*")
            .addHeader("Content-Length", "42")
            .setBody("");
        Request actual(raw);
        actual.getBody();  // lazy body
        expected.getBody();
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlPost() {
        const string raw =
            "POST /submit HTTP/1.1\r\nHost: 127.10.0.1:8888\r\nUser-Agent: curl/8.5.0\r\nAccept: "
            "*/*\r\nContent-Length: 12\r\n\r\nHello World!";
        Request expected;
        expected.setType(webserver::POST)
            .setRequestTarget("/submit")
            .setPath("/submit")
            .setVersion("HTTP/1.1")
            .addHeader("Host", "127.10.0.1:8888")
            .addHeader("User-Agent", "curl/8.5.0")
            .addHeader("Accept", "*/*")
            .addHeader("Content-Length", "12")
            .setIsBodyRaw(false)
            .setBody("Hello World!");
        Request actual(raw);
        actual.getBody();  // lazy body
        expected.getBody();
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlPostChunked() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n7\r\n World!\r\n0\r\n\r\n";
        Request expected;
        expected.setType(webserver::POST)
            .setRequestTarget("/post")
            .setPath("/post")
            .setVersion("HTTP/1.1")
            // spaces after colon skipped, spaces after the value are preserved
            .addHeader("Host", "127.10.0.1:8888 ")
            .addHeader("User-Agent", "curl/8.5.0")
            .addHeader("Transfer-Encoding", "chunked")
            .setIsBodyRaw(false)
            .setBody("Hello World!");
        Request actual(raw);
        actual.getBody();  // lazy body
        expected.getBody();
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlPostChunkedEmptyBody() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n0\r\n\r\n";
        Request expected;
        expected.setType(webserver::POST)
            .setRequestTarget("/post")
            .setPath("/post")
            .setVersion("HTTP/1.1")
            // spaces after colon skipped, spaces after the value are preserved
            .addHeader("Host", "127.10.0.1:8888 ")
            .addHeader("User-Agent", "curl/8.5.0")
            .setIsBodyRaw(false)
            .addHeader("Transfer-Encoding", "chunked");
        Request actual(raw);
        actual.getBody();  // lazy body
        expected.getBody();
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testCurlPostChunkedNoChunks() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n";
        Request act(raw);
        TS_ASSERT_THROWS(act.getBody(), webserver::IncompleteRequest);
    }

    void testCurlPostChunkedBadChunk1() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\nAA\r\n World!\r\n0\r\n\r\n";
        Request act(raw);
        TS_ASSERT_THROWS(act.getBody(), webserver::IncompleteRequest);
    }

    void testCurlPostChunkedBadChunk2() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n9\r\n World!\r\n0\r\n\r\n";
        Request act(raw);
        TS_ASSERT_THROWS(act.getBody(), webserver::BadRequest);
    }

    void testCurlPostChunkedBadChunk3() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\nA\r\n World!\r\n0\r\n\r\n";
        Request act(raw);
        TS_ASSERT_THROWS(act.getBody(), webserver::BadRequest);
    }

    void testCurlPostChunkedBadChunk4() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n6\r\n World!\r\n0\r\n\r\n";
        Request act(raw);
        TS_ASSERT_THROWS(act.getBody(), webserver::BadRequest);
    }

    void testCurlPostChunkedBodyTooLarge() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Transfer-Encoding: chunked\r\n\r\n5\r\nHello\r\n7\r\n World!\r\n0\r\n\r\n";
        Request act(raw);
        act.setMaxClientBodySizeBytes(4);
        TS_ASSERT_THROWS(act.getBody(), webserver::PayloadTooLarge);
    }

    void testCurlPostBodyTooLarge() {
        const string raw =
            "POST /post HTTP/1.1\r\nHost:   127.10.0.1:8888 \r\nUser-Agent: curl/8.5.0\r\n"
            "Content-Length: 5\r\n\r\nHello";
        Request act(raw);
        act.setMaxClientBodySizeBytes(4);
        TS_ASSERT_THROWS(act.getBody(), webserver::PayloadTooLarge);
    }

    void testIncompleteBody() {
        const string raw =
            "POST /submit HTTP/1.1\r\nHost: 127.10.0.1:8888\r\nUser-Agent: curl/8.5.0\r\nAccept: "
            "*/*\r\nContent-Length: 12\r\n\r\nHello Wo";
        Request act(raw);
        TS_ASSERT_THROWS(act.getBody(), webserver::IncompleteRequest);
    }

    void testExcessiveBody() {
        const string raw =
            "POST /submit HTTP/1.1\r\nHost: 127.10.0.1:8888\r\nUser-Agent: curl/8.5.0\r\nAccept: "
            "*/*\r\nContent-Length: 1\r\n\r\nHello World!";
        Request act(raw);
        TS_ASSERT_THROWS(act.getBody(), webserver::BadRequest);
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
