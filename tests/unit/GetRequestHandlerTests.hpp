#ifndef GETREQUESTHANDLERTESTS_HPP
#define GETREQUESTHANDLERTESTS_HPP

#include <cxxtest/TestSuite.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "configuration/AppConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "configuration/FolderConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "http_methods/HttpMethodType.hpp"
#include "request_handler/GetHandler.hpp"

using std::map;
using std::ofstream;
using std::set;
using std::string;

class GetRequestHandlerTests : public CxxTest::TestSuite {
private:
    static string _rootFolder;
    static map<string, string> _files;  // path: content
    static set<string> _emptyFolders;

    bool makePath(const string& path) {
        size_t pos = 0;
        do {
            pos = path.find('/', pos + 1);
            string sub = path.substr(0, pos);
            if (!sub.empty() && pos != string::npos) {
                if (mkdir(sub.c_str(), 0755) != 0 && errno != EEXIST)
                    return false;
            }
        } while (pos != string::npos);

        return true;
    }

    void createTestFiles() {
        for (set<string>::iterator it = _emptyFolders.begin(); it != _emptyFolders.end(); it++) {
            makePath(_rootFolder + it->c_str());
        }
        for (map<string, string>::iterator it = _files.begin(); it != _files.end(); it++) {
            makePath(_rootFolder + it->first.c_str());
            string p = _rootFolder + it->first;
            ofstream f(p.c_str());
            f << it->second;
            f.close();
        }
    }

public:
    void testThatGetFetchesFilesAndGeneratesCorrectResponseHeaders() {
        _files["/folder/foo.txt"] = "footext";
        _files["/folder/bar.xml"] = "bartext";
        _files["/another/key.jpg"] = "communication";
        _files["/another/empty.mp3"] = "";
        _emptyFolders.insert("/folder/empty");
        _emptyFolders.insert("/another/empty");
        _emptyFolders.insert("/third");
        createTestFiles();
        webserver::Endpoint config =
            webserver::Endpoint().addRoute(webserver::RouteConfig().setPath("/").setFolderConfig(
                webserver::FolderConfig(_rootFolder, false, "index.html")
            ));

        webserver::Response actual = webserver::GetHandler::handleRequest("folder/foo.txt", config);
        TS_ASSERT_EQUALS(200, actual.getStatus());
        TS_ASSERT_EQUALS("7", actual.getHeader("Content-Length"));
        TS_ASSERT_EQUALS("footext", actual.getBody());
        TS_ASSERT_EQUALS("text/plain", actual.getHeader("Content-Type"));

        actual = webserver::GetHandler::handleRequest("folder/bar.xml", config);
        TS_ASSERT_EQUALS(200, actual.getStatus());
        TS_ASSERT_EQUALS("7", actual.getHeader("Content-Length"));
        TS_ASSERT_EQUALS("bartext", actual.getBody());
        TS_ASSERT_EQUALS("application/xml", actual.getHeader("Content-Type"));

        actual = webserver::GetHandler::handleRequest("another/key.jpg", config);
        TS_ASSERT_EQUALS(200, actual.getStatus());
        TS_ASSERT_EQUALS("13", actual.getHeader("Content-Length"));
        TS_ASSERT_EQUALS("communication", actual.getBody());
        TS_ASSERT_EQUALS("image/jpeg", actual.getHeader("Content-Type"));

        actual = webserver::GetHandler::handleRequest("another/empty.mp3", config);
        TS_ASSERT_EQUALS(200, actual.getStatus());
        TS_ASSERT_EQUALS("0", actual.getHeader("Content-Length"));
        TS_ASSERT_EQUALS("", actual.getBody());
        TS_ASSERT_EQUALS("audio/mpeg", actual.getHeader("Content-Type"));

        actual = webserver::GetHandler::handleRequest("another/doesnotexist.txt", config);
        TS_ASSERT_EQUALS(404, actual.getStatus());
        TS_ASSERT_EQUALS("798", actual.getHeader("Content-Length"));
        TS_ASSERT_EQUALS(
            "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n    <meta charset=\"UTF-8\">\n    "
            "<title>404 Not Found</title>\n    <style>\n        body {\n            margin: 0;\n   "
            "         "
            "height: 100vh;\n            background: #000000;\n            color: #ffffff;\n       "
            "     "
            "font-family: Helvetica, Arial, sans-serif;\n            display: flex;\n            "
            "align-items: center;\n            justify-content: center;\n        }\n        .box "
            "{\n            "
            "text-align: center;\n        }\n        h1 {\n            font-size: 6rem;\n          "
            "  "
            "margin: 0;\n        }\n        p {\n            margin-top: 1rem;\n            "
            "font-size: 1.1rem;\n            opacity: 0.9;\n        }\n    "
            "</style>\n</head>\n<body>\n    "
            "<div class=\"box\">\n        <h1>404</h1>\n        <p>The page you were looking for "
            "doesn't exist.</p>\n    "
            "</div>\n</body>\n</html>",
            actual.getBody()
        );
        TS_ASSERT_EQUALS("text/html", actual.getHeader("Content-Type"));
    }

    // deletes test files
    void tearDown() {
        string cmd = "rm -rf '" + _rootFolder + "'";
        system(cmd.c_str());
    }
};

string GetRequestHandlerTests::_rootFolder = "testRoot";
map<string, string> GetRequestHandlerTests::_files;
set<string> GetRequestHandlerTests::_emptyFolders;
#endif
