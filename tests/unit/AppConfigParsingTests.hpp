#ifndef CONFIGPARSINGTESTS_HPP
#define CONFIGPARSINGTESTS_HPP

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

class AppConfigParsingTests : public CxxTest::TestSuite {
private:
    set<string> _configFilenames;

public:
    void testConfig0() {
        const string fname = "config0.tst";
        _configFilenames.insert(fname);
        ofstream f("config0.tst");
        f << "server {\n"
          << "\tlisten 8080;\n"
          << "\tserver_name localhost;\n"
          << "\tlocation / {\n"
          << "\t\troot /var/www/html;\n"
          << "\t\tindex index.html;\n"
          << "\t}\n"
          << "}" << endl;
        f.close();
        // TODO 15: probably should call the parser class, not instantiate the server, since it adds extra log output to the logs
        webserver::WebServer& s = webserver::WebServer::getInstance(fname);

        webserver::AppConfig expected;
        expected.addEndpoint(webserver::Endpoint("localhost", 8080))
            .addRoute(
                "/",
                webserver::RouteConfig().setFolderConfig(
                    webserver::FolderConfig("/", false, "index.html")
                )
            );
        TS_SKIP("configuration parsing not implemented");
        TS_ASSERT_THROWS_NOTHING(s.getAppConfig());
        TS_ASSERT_EQUALS(expected, s.getAppConfig());
    }

    void tearDown() {
        for (set<string>::iterator it = _configFilenames.begin(); it != _configFilenames.end();
             it++) {
            if (std::remove(it->c_str()) != 0) {
                std::cerr << "Failed to remove file " << it->c_str() << "\n";
            }
        }
    }
};
#endif
