#include <exception>
#include <string>
#include <iostream>

#include "WebServer.hpp"
#include "configuration/parser/ConfigParsingException.hpp"
#include "logger/Logger.hpp"

using std::cerr;
using std::endl;

int main(int argc, char* argv[]) {
    webserver::Logger _log;
    if (argc != 2) {
        _log.stream(LOG_FATAL) << "Failed to launch: no config file provided.\n"
                               << "Usage:   " << argv[0] << " <config_file>\n"
                               << "Example: " << argv[0]
                               << " ./tests/config_files/local_run.conf\n";
        return (1);
    }
    try {
        webserver::WebServer::getInstance(argv[1]);
    } catch (const webserver::ConfigParsingException& e) {
        std::cerr << "Malformed configuration file, aborting startup: " << e.what() << std::endl;
        return (1);
    } catch (const std::exception& e) {
        std::cerr << "Fatal runtime error: " << e.what() << std::endl;
        return (1);
    }
    try {
        webserver::WebServer& server = webserver::WebServer::getInstance(argv[1]);
        server.start();
    } catch (const std::exception& e) {
        std::cerr << "Fatal runtime error: " << e.what() << std::endl;
        return (1);
    }
    return (0);
}
