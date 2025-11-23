#include "WebServer.hpp"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    // NOTE: creates a single webserver instance (singleton)
    webserver::WebServer& server = webserver::WebServer::getInstance("");
    server.start();

    return (0);
}
