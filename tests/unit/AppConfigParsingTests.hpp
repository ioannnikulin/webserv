#ifndef CONFIGPARSINGTESTS_HPP
#define CONFIGPARSINGTESTS_HPP

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

using std::endl;
using std::ofstream;
using std::set;
using std::string;

class AppConfigParsingTests : public CxxTest::TestSuite {
private:
    set<string> _configFilenames;

    void printDebugInfo(const webserver::AppConfig& expected, const webserver::AppConfig& actual) {
        const std::set<webserver::Endpoint>& epActual =
            const_cast<webserver::AppConfig&>(actual).getEndpoints();
        const std::set<webserver::Endpoint>& epExpected =
            const_cast<webserver::AppConfig&>(expected).getEndpoints();

        std::cout << "\n=== Configuration Comparison ===" << std::endl;
        std::cout << "Actual endpoints count:   " << epActual.size() << std::endl;
        std::cout << "Expected endpoints count: " << epExpected.size() << std::endl;

        std::set<webserver::Endpoint>::const_iterator itA = epActual.begin();
        std::set<webserver::Endpoint>::const_iterator itE = epExpected.begin();

        int endpointNum = 0;
        while (itA != epActual.end() && itE != epExpected.end()) {
            std::cout << "\n--- Endpoint " << endpointNum << " Comparison ---" << std::endl;
            std::cout << "Actual interface:   '" << itA->getInterface() << "'" << std::endl;
            std::cout << "Expected interface: '" << itE->getInterface() << "'" << std::endl;
            std::cout << "Match: " << (itA->getInterface() == itE->getInterface() ? "YES" : "NO")
                      << std::endl;

            std::cout << "\nActual port:   " << itA->getPort() << std::endl;
            std::cout << "Expected port: " << itE->getPort() << std::endl;
            std::cout << "Match: " << (itA->getPort() == itE->getPort() ? "YES" : "NO")
                      << std::endl;

            const std::vector<webserver::RouteConfig>& routesActual = itA->getRoutes();
            const std::vector<webserver::RouteConfig>& routesExpected = itE->getRoutes();

            std::cout << "\n--- Routes Comparison ---" << std::endl;
            std::cout << "Actual routes count:   " << routesActual.size() << std::endl;
            std::cout << "Expected routes count: " << routesExpected.size() << std::endl;
            std::cout << "Match: " << (routesActual.size() == routesExpected.size() ? "YES" : "NO")
                      << std::endl;

            size_t minSize = routesActual.size() < routesExpected.size() ? routesActual.size()
                                                                         : routesExpected.size();
            for (size_t i = 0; i < minSize; ++i) {
                std::cout << "\n--- Route[" << i << "] Details ---" << std::endl;
                std::cout << "Actual path:   '" << routesActual[i].getPath() << "'" << std::endl;
                std::cout << "Expected path: '" << routesExpected[i].getPath() << "'" << std::endl;
                std::cout << "Match: "
                          << (routesActual[i].getPath() == routesExpected[i].getPath() ? "YES"
                                                                                       : "NO")
                          << std::endl;

                try {
                    bool routesEqual = (routesActual[i] == routesExpected[i]);
                    std::cout << "Routes equal: " << (routesEqual ? "YES" : "NO") << std::endl;
                } catch (...) {
                    std::cout << "EXCEPTION caught during route comparison!" << std::endl;
                }
            }

            std::cout << "\n--- Endpoint Equality Check ---" << std::endl;
            std::cout << "Endpoints equal: " << (*itA == *itE ? "YES" : "NO") << std::endl;

            ++itA;
            ++itE;
            ++endpointNum;
        }

        std::cout << "\n--- AppConfig Equality Check ---" << std::endl;
        std::cout << "AppConfigs equal: " << (expected == actual ? "YES" : "NO") << std::endl;
        std::cout << "================================\n" << std::endl;
    }

public:
    void setUp() {
        webserver::HttpStatus::initStatusMap();
    }

    void testConfig0_BasicServer() {
        const string fname = "config0.tst";
        _configFilenames.insert(fname);
        ofstream f(fname.c_str());
        f << "server {\n"
          << "\tlisten 127.0.0.1:8080;\n"
          << "\tserver_name localhost;\n"
          << "\tlocation / {\n"
          << "\t\troot /var/www/html;\n"
          << "\t\tindex index.html;\n"
          << "\t}\n"
          << "}" << endl;
        f.close();

        webserver::ConfigParser parser;
        webserver::AppConfig actual = parser.parse(fname);

        webserver::AppConfig expected;
        webserver::Endpoint ep("127.0.0.1", 8080);

        std::string serverName = "localhost";
        ep.addServerName(serverName);

        webserver::RouteConfig route;
        route.setPath("/");
        route.setFolderConfig(webserver::FolderConfig("/var/www/html", false, "index.html"));

        ep.addRoute(route);
        expected.addEndpoint(ep);

        //printDebugInfo(expected, actual);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testConfig1_NestedLocations() {
        const string fname = "nested_locations.conf";
        _configFilenames.insert(fname);
        ofstream f(fname.c_str());
        f << "server {\n"
          << "    listen 8081;\n"
          << "    server_name nested.local;\n"
          << "\n"
          << "    location / {\n"
          << "        root /srv/www;\n"
          << "        index index.html;\n"
          << "    }\n"
          << "\n"
          << "    location /admin {\n"
          << "        root /srv/www/admin;\n"
          << "        index dashboard.html;\n"
          << "        methods GET POST DELETE;\n"
          << "    }\n"
          << "\n"
          << "    location /uploads {\n"
          << "        root /srv/www/uploads;\n"
          << "        upload on /srv/uploads;\n"
          << "    }\n"
          << "}" << endl;
        f.close();

        webserver::ConfigParser parser;
        webserver::AppConfig actual = parser.parse(fname);

        webserver::AppConfig expected;
        webserver::Endpoint ep("0.0.0.0", 8081);

        std::string serverName = "nested.local";
        ep.addServerName(serverName);
        // Location /
        webserver::RouteConfig route1;
        route1.setPath("/");
        route1.setFolderConfig(webserver::FolderConfig("/srv/www", false, "index.html"));
        ep.addRoute(route1);

        // Location /admin
        webserver::RouteConfig route2;
        route2.setPath("/admin");
        route2.setFolderConfig(webserver::FolderConfig("/srv/www/admin", false, "dashboard.html"));
        route2.addAllowedMethod(webserver::GET);
        route2.addAllowedMethod(webserver::POST);
        route2.addAllowedMethod(webserver::DELETE);
        ep.addRoute(route2);

        // Location /uploads
        webserver::RouteConfig route3;
        route3.setPath("/uploads");
        route3.setFolderConfig(webserver::FolderConfig("/srv/www/uploads", false, ""));
        route3.setUploadConfig(webserver::UploadConfig(true, "/srv/uploads"));
        ep.addRoute(route3);

        expected.addEndpoint(ep);

        //printDebugInfo(expected, actual);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testConfig2_MultiServer() {
        const string fname = "multi_server.conf";
        _configFilenames.insert(fname);
        ofstream f(fname.c_str());
        f << "server {\n"
          << "    listen 8080;\n"
          << "    server_name example.com;\n"
          << "\n"
          << "    location / {\n"
          << "        root /var/www/example;\n"
          << "        index index.html;\n"
          << "    }\n"
          << "}\n"
          << "\n"
          << "server {\n"
          << "    listen 9090;\n"
          << "    server_name api.localhost;\n"
          << "\n"
          << "    location /api {\n"
          << "        root /var/www/api;\n"
          << "        index index.json;\n"
          << "    }\n"
          << "}" << endl;
        f.close();

        webserver::ConfigParser parser;
        webserver::AppConfig actual = parser.parse(fname);

        webserver::AppConfig expected;

        // First server
        webserver::Endpoint ep1("0.0.0.0", 8080);
        webserver::RouteConfig route1;
        std::string serverName1 = "example.com";
        ep1.addServerName(serverName1);
        route1.setPath("/");
        route1.setFolderConfig(webserver::FolderConfig("/var/www/example", false, "index.html"));
        ep1.addRoute(route1);
        expected.addEndpoint(ep1);

        // Second server
        webserver::Endpoint ep2("0.0.0.0", 9090);
        webserver::RouteConfig route2;
        std::string serverName2 = "api.localhost";
        ep2.addServerName(serverName2);
        route2.setPath("/api");
        route2.setFolderConfig(webserver::FolderConfig("/var/www/api", false, "index.json"));
        ep2.addRoute(route2);
        expected.addEndpoint(ep2);

        //printDebugInfo(expected, actual);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testConfig3_CgiHandler() {
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

        std::string serverName = "cgi.local";
        ep.addServerName(serverName);
        webserver::RouteConfig route;
        route.setPath("/");
        route.setFolderConfig(webserver::FolderConfig("/srv/www/cgi", false, "index.py"));
        ep.addRoute(route);

        // Add CGI handlers
        ep.addCgiHandler(webserver::CgiHandlerConfig(30, "/usr/bin/python3"), ".py");
        ep.addCgiHandler(webserver::CgiHandlerConfig(30, "/usr/bin/php-cgi"), ".php");

        expected.addEndpoint(ep);

        //printDebugInfo(expected, actual);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void testConfig4_Advanced() {
        const string fname = "advanced.conf";
        _configFilenames.insert(fname);
        ofstream f(fname.c_str());
        f << "server {\n"
          << "    listen 443;\n"
          << "    server_name secure.example.com;\n"
          << "\n"
          << "    error_page 404 /errors/404.html;\n"
          << "    error_page 500 /errors/500.html;\n"
          << "\n"
          << "    client_max_body_size 1M;\n"
          << "\n"
          << "    location / {\n"
          << "        root /srv/secure;\n"
          << "        index index.html;\n"
          << "    }\n"
          << "\n"
          << "    location /upload {\n"
          << "        root /srv/uploads;\n"
          << "        methods POST;\n"
          << "        upload on /srv/uploads/tmp;\n"
          << "    }\n"
          << "\n"
          << "    location /redirect {\n"
          << "        return http://example.com;\n"
          << "    }\n"
          << "}" << endl;
        f.close();

        webserver::ConfigParser parser;
        webserver::AppConfig actual = parser.parse(fname);

        webserver::AppConfig expected;
        webserver::Endpoint ep("0.0.0.0", 443);
        std::string serverName = "secure.example.com";

        ep.addServerName(serverName);
        ep.setClientMaxBodySize(1 * webserver::ConfigParser::MIB);
        // Location /
        webserver::RouteConfig route1;
        route1.setPath("/");
        route1.setFolderConfig(webserver::FolderConfig("/srv/secure", false, "index.html"));
        ep.addRoute(route1);

        // Location /upload
        webserver::RouteConfig route2;
        route2.setPath("/upload");
        route2.setFolderConfig(webserver::FolderConfig("/srv/uploads", false, ""));
        route2.addAllowedMethod(webserver::POST);
        route2.setUploadConfig(webserver::UploadConfig(true, "/srv/uploads/tmp"));
        ep.addRoute(route2);

        // Location /redirect
        webserver::RouteConfig route3;
        route3.setPath("/redirect");
        route3.setFolderConfig(webserver::FolderConfig("", false, ""));
        route3.addRedirection("/redirect", "http://example.com");
        ep.addRoute(route3);

        expected.addEndpoint(ep);

        printDebugInfo(expected, actual);
        TS_ASSERT_EQUALS(expected, actual);
    }

    void tearDown() {
        for (set<string>::iterator it = _configFilenames.begin(); it != _configFilenames.end();
             it++) {
            if (std::remove(it->c_str()) != 0) {
                // Only print error if file should exist but deletion failed
                // Ignore "file not found" errors
                // std::cerr << "Failed to remove file " << it->c_str() << "\n";
            }
        }
        _configFilenames.clear();
    }
};

#endif
