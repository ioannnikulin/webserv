#include "WebServer.hpp"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    webserver::WebServer& server = webserver::WebServer::getInstance(argv[1]);
    server.start();

    return (0);
}
