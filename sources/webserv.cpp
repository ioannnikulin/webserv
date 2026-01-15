#include <exception>
#include <string>

#include "WebServer.hpp"
#include "configuration/parser/ConfigParsingException.hpp"
#include "logger/Logger.hpp"

int main(int argc, char* argv[]) {
    webserver::Logger log;
    if (argc != 2) {
        log.stream(LOG_FATAL) << "Failed to launch: no config file provided.\n"
                              << "Usage:   " << argv[0] << " <config_file>\n"
                              << "Example: " << argv[0] << " ./tests/config_files/local_run.conf\n";
        return (1);
    }
    try {
        webserver::WebServer::getInstance(argv[1]);
    } catch (const webserver::ConfigParsingException& e) {
        log.stream(LOG_FATAL) << "Malformed configuration file, aborting startup: " << e.what()
                              << "\n";
        return (1);
    } catch (const std::exception& e) {
        log.stream(LOG_FATAL) << "Fatal runtime error: " << e.what() << "\n";
        return (1);
    }
    try {
        webserver::WebServer& server = webserver::WebServer::getInstance(argv[1]);
        server.start();
    } catch (const std::exception& e) {
        log.stream(LOG_FATAL) << "Fatal runtime error: " << e.what() << "\n";
        return (1);
    }
    return (0);
}
