#include "WebServer.hpp"

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    webserver::WebServer& server = webserver::WebServer::getInstance("");
    server.start();
    return (0);
}
