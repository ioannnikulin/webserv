#ifndef APPCONFIGPARSINGTESTS_HPP
#define APPCONFIGPARSINGTESTS_HPP

#include <cxxtest/TestSuite.h>

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "WebServer.hpp"
#include "configuration/RouteConfig.hpp"
#include "configuration/parser/ConfigParser.hpp"
#include "http_status/HttpStatus.hpp"
#include "utils/utils.hpp"

using std::cout;
using std::endl;
using std::ofstream;
using std::set;
using std::string;
using std::vector;

class AppConfigParsingTests : public CxxTest::TestSuite {
private:
    set<string> _configFilenames;

    void printDebugInfo(const webserver::AppConfig& expected, const webserver::AppConfig& actual) {
        const set<webserver::Endpoint>& epActual =
            const_cast<webserver::AppConfig&>(actual).getEndpoints();
        const set<webserver::Endpoint>& epExpected =
            const_cast<webserver::AppConfig&>(expected).getEndpoints();

        cout << "\n=== Configuration Comparison ===" << endl;
        cout << "Actual endpoints count:   " << epActual.size() << endl;
        cout << "Expected endpoints count: " << epExpected.size() << endl;

        set<webserver::Endpoint>::const_iterator itA = epActual.begin();
        set<webserver::Endpoint>::const_iterator itE = epExpected.begin();

        int endpointNum = 0;
        while (itA != epActual.end() && itE != epExpected.end()) {
            cout << "\n--- Endpoint " << endpointNum << " Comparison ---" << endl;
            cout << "Actual interface:   '" << itA->getInterface() << "'" << endl;
            cout << "Expected interface: '" << itE->getInterface() << "'" << endl;
            cout << "Match: " << (itA->getInterface() == itE->getInterface() ? "YES" : "NO")
                 << endl;

            cout << "\nActual port:   " << itA->getPort() << endl;
            cout << "Expected port: " << itE->getPort() << endl;
            cout << "Match: " << (itA->getPort() == itE->getPort() ? "YES" : "NO") << endl;

            const set<webserver::RouteConfig>& routesActual = itA->getRoutes();
            const set<webserver::RouteConfig>& routesExpected = itE->getRoutes();

            cout << "\n--- Routes Comparison ---" << endl;
            cout << "Actual routes count:   " << routesActual.size() << endl;
            cout << "Expected routes count: " << routesExpected.size() << endl;
            cout << "Match: " << (routesActual.size() == routesExpected.size() ? "YES" : "NO")
                 << endl;

            size_t minSize = routesActual.size() < routesExpected.size() ? routesActual.size()
                                                                         : routesExpected.size();
            set<webserver::RouteConfig>::const_iterator itRA = routesActual.begin();
            set<webserver::RouteConfig>::const_iterator itRE = routesExpected.begin();
            for (size_t i = 0; i < minSize; ++i, ++itRA, ++itRE) {
                cout << "\n--- Route[" << i << "] Details ---" << endl;
                cout << "Actual path:   '" << itRA->getPath() << "'" << endl;
                cout << "Expected path: '" << itRE->getPath() << "'" << endl;
                cout << "Match: " << (itRA->getPath() == itRE->getPath() ? "YES" : "NO") << endl;
                try {
                    bool routesEqual = (*itRA == *itRE);
                    cout << "Routes equal: " << (routesEqual ? "YES" : "NO") << endl;
                } catch (...) {
                    cout << "EXCEPTION caught during route comparison!" << endl;
                }
            }

            cout << "\n--- Endpoint Equality Check ---" << endl;
            cout << "Endpoints equal: " << (*itA == *itE ? "YES" : "NO") << endl;

            ++itA;
            ++itE;
            ++endpointNum;
        }

        cout << "\n--- AppConfig Equality Check ---" << endl;
        cout << "AppConfigs equal: " << (expected == actual ? "YES" : "NO") << endl;
        cout << "================================\n" << endl;
    }

public:
    void setUp() {
        webserver::HttpStatus::initStatusMap();
    }

    void testConfig0_BasicServer() {
        const string fname = "tests/config_files/basic.conf";

        webserver::ConfigParser parser;
        webserver::AppConfig actual = parser.parse(fname);

        webserver::AppConfig expected;
        webserver::Endpoint ep("127.1.0.1", 8080);

        string serverName = "localhost";
        ep.addServerName(serverName);

        webserver::RouteConfig route;
        route.setPath("/");
        route.setFolderConfig(webserver::FolderConfig(
            "/",
            "tests/e2e/5/requirements/webserv/volume/www/html",
            false,
            "index.html",
            webserver::FolderConfig::defaultMaxClientBodySizeBytes()
        ));

        ep.addRoute(route);
        expected.addEndpoint(ep);

        //printDebugInfo(expected, actual);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testConfig1_NestedLocations() {
        const string fname = "tests/config_files/nested_locations.conf";

        webserver::ConfigParser parser;
        webserver::AppConfig actual = parser.parse(fname);

        webserver::AppConfig expected;
        webserver::Endpoint ep("0.0.0.0", 8081);

        string serverName = "nested.local";
        ep.addServerName(serverName);
        ep.setMaxClientBodySizeBytes(1000);
        // Location /
        webserver::RouteConfig route1;
        route1.setPath("/");
        route1.setFolderConfig(webserver::FolderConfig(
            "/",
            "tests/e2e/6/requirements/webserv/volume/www",
            false,
            "index.html",
            2000
        ));
        ep.addRoute(route1);

        // Location /admin
        webserver::RouteConfig route2;
        route2.setPath("/admin");
        route2.setFolderConfig(webserver::FolderConfig(
            "/admin",
            "tests/e2e/6/requirements/webserv/volume/www/admin",
            false,
            "dashboard.html",
            1000
        ));
        route2.addAllowedMethod(webserver::GET);
        route2.addAllowedMethod(webserver::POST);
        route2.addAllowedMethod(webserver::DELETE);
        ep.addRoute(route2);

        // Location /uploads
        webserver::RouteConfig route3;
        route3.setPath("/uploads");
        route3.setFolderConfig(webserver::FolderConfig(
            "/uploads",
            "tests/e2e/6/requirements/webserv/volume/www/uploads",
            false,
            "",
            1000
        ));
        route3.setUploadConfig(
            webserver::UploadConfig(true, "tests/e2e/6/requirements/webserv/volume/www/uploads")
        );
        ep.addRoute(route3);

        expected.addEndpoint(ep);

        //printDebugInfo(expected, actual);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testConfig2_MultiServer() {
        const string fname = "tests/config_files/multi_server.conf";

        webserver::ConfigParser parser;
        webserver::AppConfig actual = parser.parse(fname);

        webserver::AppConfig expected;

        // First server
        webserver::Endpoint ep1("0.0.0.0", 8080);
        webserver::RouteConfig route1;
        string serverName1 = "example.com";
        ep1.addServerName(serverName1);
        route1.setPath("/");
        route1.setFolderConfig(webserver::FolderConfig(
            "/",
            "tests/e2e/7/requirements/webserv/volume/www/example",
            false,
            "index.html",
            webserver::FolderConfig::defaultMaxClientBodySizeBytes()
        ));
        ep1.addRoute(route1);
        expected.addEndpoint(ep1);

        // Second server
        webserver::Endpoint ep2("0.0.0.0", 9090);
        webserver::RouteConfig route2;
        string serverName2 = "api.localhost";
        ep2.addServerName(serverName2);
        route2.setPath("/api");
        route2.setFolderConfig(webserver::FolderConfig(
            "/api",
            "tests/e2e/7/requirements/webserv/volume/www/api",
            false,
            "index.html",
            webserver::FolderConfig::defaultMaxClientBodySizeBytes()
        ));
        ep2.addRoute(route2);
        expected.addEndpoint(ep2);

        //printDebugInfo(expected, actual);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testConfig3_CgiHandler() {
        TS_SKIP("Temporarily disabled: CGI handler config is under refactor");

        const string fname = "cgi_example.conf";
        _configFilenames.insert(fname);
        ofstream f(fname.c_str());
        f << "server {\n"
          << "    listen 8000;\n"
          << "    server_name cgi.local;\n"
          << "\n"
          << "    location / {\n"
          << "        root /srv/www/cgi;\n"
          << "        index index.py;\n"
          << "    }\n"
          << "\n"
          << "    cgi .py /usr/bin/python3;\n"
          << "    cgi .php /usr/bin/php-cgi;\n"
          << "}" << endl;
        f.close();

        webserver::ConfigParser parser;
        webserver::AppConfig actual = parser.parse(fname);

        webserver::AppConfig expected;
        webserver::Endpoint ep("0.0.0.0", 8000);

        string serverName = "cgi.local";
        ep.addServerName(serverName);
        webserver::RouteConfig route;
        route.setPath("/");
        route.setFolderConfig(webserver::FolderConfig(
            "/",
            "/srv/www/cgi",
            false,
            "index.py",
            webserver::FolderConfig::defaultMaxClientBodySizeBytes()
        ));
        ep.addRoute(route);

        // Add CGI handlers
        ep.addCgiHandler(webserver::CgiHandlerConfig(30, "/usr/bin/python3"), ".py");
        ep.addCgiHandler(webserver::CgiHandlerConfig(30, "/usr/bin/php-cgi"), ".php");

        expected.addEndpoint(ep);

        //printDebugInfo(expected, actual);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testConfig4_Advanced() {
        const string fname = "tests/config_files/advanced.conf";

        webserver::ConfigParser parser;
        webserver::AppConfig actual = parser.parse(fname);

        webserver::AppConfig expected;
        webserver::Endpoint ep("0.0.0.0", 443);
        string serverName = "secure.example.com";

        ep.addServerName(serverName);
        ep.setMaxClientBodySizeBytes(1 * utils::MIB);

        // Location /
        webserver::RouteConfig route1;
        route1.setPath("/");
        route1.setFolderConfig(webserver::FolderConfig(
            "/",
            "tests/e2e/4/requirements/webserv/volume/secure",
            false,
            "index.html",
            1 * utils::MIB
        ));
        ep.addRoute(route1);

        // Location /upload
        webserver::RouteConfig route2;
        route2.setPath("/upload");
        route2.setFolderConfig(webserver::FolderConfig(
            "/upload",
            "tests/e2e/4/requirements/webserv/volume/uploads",
            false,
            "",
            1 * utils::MIB
        ));
        route2.addAllowedMethod(webserver::POST);
        route2.setUploadConfig(
            webserver::UploadConfig(true, "tests/e2e/4/requirements/webserv/volume/uploads/tmp")
        );
        ep.addRoute(route2);

        // Location /redirect
        webserver::RouteConfig route3;
        route3.setPath("/redirect");
        route3.setFolderConfig(webserver::FolderConfig("/redirect", "", false, "", 1 * utils::MIB));
        route3.setRedirection("http://example.com");
        ep.addRoute(route3);

        expected.addEndpoint(ep);

        //printDebugInfo(expected, actual);
        TS_ASSERT_EQUALS(expected, actual);
    }
};

#endif
