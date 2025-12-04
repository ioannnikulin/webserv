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

#include "request_handler/RequestHandler.hpp"

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
            if (!sub.empty()) {
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
    void test0() {
        _files["/folder/foo.txt"] = "footext";
        _files["/folder/bar.txt"] = "bartext";
        _files["/another/key.txt"] = "communication";
        _files["/another/empty.txt"] = "";
        _emptyFolders.insert("/folder/empty");
        _emptyFolders.insert("/another/empty");
        _emptyFolders.insert("/third");
        createTestFiles();

        TS_SKIP("request handling not implemented");

        string actual = webserver::RequestHandler::handle("GET", "/folder/foo.txt");
        TS_ASSERT_EQUALS("footext", actual);
        actual = webserver::RequestHandler::handle("GET", "/folder/bar.txt");
        TS_ASSERT_EQUALS("bartext", actual);
        actual = webserver::RequestHandler::handle("GET", "/another/key.txt");
        TS_ASSERT_EQUALS("communication", actual);
        actual = webserver::RequestHandler::handle("GET", "/another/empty.txt");
        TS_ASSERT_EQUALS("", actual);
        TS_ASSERT_THROWS(
            webserver::RequestHandler::handle("GET", "/folder/doesntexist.txt"),
            std::runtime_error  // TODO 58: actually should declare some more specific exception
        );
    }

    void tearDown() {
        string cmd = "rm -rf '" + _rootFolder + "'";
        system(cmd.c_str());
    }
};

string GetRequestHandlerTests::_rootFolder = "testRoot";
map<string, string> GetRequestHandlerTests::_files;
set<string> GetRequestHandlerTests::_emptyFolders;
#endif
