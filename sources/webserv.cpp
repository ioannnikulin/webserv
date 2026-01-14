#include <string>

#include "WebServer.hpp"
#include "logger/Logger.hpp"

int main(int argc, char* argv[]) {
    webserver::Logger _log;
    if (argc != 2) {
        _log.stream(LOG_FATAL) << "Failed to launch: no config file provided.\n"
                               << "Usage:   " << argv[0] << " <config_file>\n"
                               << "Example: " << argv[0]
                               << " ./tests/config_files/local_run.conf\n";
        return (1);
    }
    webserver::WebServer& server = webserver::WebServer::getInstance(argv[1]);
    server.start();

    return (0);
}
