#include <string>

#include "WebServer.hpp"
#include "logger/Logger.hpp"

int main(int argc, char* argv[]) {
    webserver::Logger log;
    if (argc != 2) {
        log.stream(LOG_FATAL) << "Failed to launch, no config file provided. Usage: " << argv[0]
                              << " <config_file>\n";
        return (1);
    }
    webserver::WebServer& server = webserver::WebServer::getInstance(argv[1]);
    server.start();

    return (0);
}
