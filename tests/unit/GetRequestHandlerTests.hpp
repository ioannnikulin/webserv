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

#include "request_handler/GetHandler.hpp"

using std::clog;
using std::endl;
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
        _files["/folder/bar.txt"] = "bartext";
        _files["/another/key.txt"] = "communication";
        _files["/another/empty.txt"] = "";
        _emptyFolders.insert("/folder/empty");
        _emptyFolders.insert("/another/empty");
        _emptyFolders.insert("/third");
        createTestFiles();

        ResponseData actual = webserver::GetHandler::handleRequest("folder/foo.txt", _rootFolder);
        TS_ASSERT_EQUALS(200, actual.statusCode);
        TS_ASSERT_EQUALS(7, actual.contentLength);
        TS_ASSERT_EQUALS("footext", actual.body);
        TS_ASSERT_EQUALS("text/plain", actual.contentType);

        actual = webserver::GetHandler::handleRequest("folder/bar.txt", _rootFolder);
        TS_ASSERT_EQUALS(200, actual.statusCode);
        TS_ASSERT_EQUALS(7, actual.contentLength);
        TS_ASSERT_EQUALS("bartext", actual.body);
        TS_ASSERT_EQUALS("text/plain", actual.contentType);

        actual = webserver::GetHandler::handleRequest("another/key.txt", _rootFolder);
        TS_ASSERT_EQUALS(200, actual.statusCode);
        TS_ASSERT_EQUALS(13, actual.contentLength);
        TS_ASSERT_EQUALS("communication", actual.body);
        TS_ASSERT_EQUALS("text/plain", actual.contentType);

        actual = webserver::GetHandler::handleRequest("another/empty.txt", _rootFolder);
        TS_ASSERT_EQUALS(200, actual.statusCode);
        TS_ASSERT_EQUALS(0, actual.contentLength);
        TS_ASSERT_EQUALS("", actual.body);
        TS_ASSERT_EQUALS("text/plain", actual.contentType);

        actual = webserver::GetHandler::handleRequest("another/doesnotexist.txt", _rootFolder);
        TS_ASSERT_EQUALS(404, actual.statusCode);
        TS_ASSERT_EQUALS(194, actual.contentLength);
        TS_ASSERT_EQUALS(
            "<html>\n    <head>\n\t\t<style>\n\t\t\tbody {\n\t\t\t\tbackground-color: "
            "pink;\n\t\t\t}\n\t\t</style>\n        <title>404</title>\n    </head>\n    <body>\n   "
            "     <h1>404. Resource not found.</h1>\n    </body>\n</html>\n",
            actual.body
        );
        TS_ASSERT_EQUALS("text/html", actual.contentType);
        // 4 asserts
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
