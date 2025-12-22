#include <iostream>

#include "WebServer.hpp"

using std::cerr;
using std::endl;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <config_file>\nFor example, " << argv[0]
             << " tests/config_files/local_run.conf" << endl;
        return (1);
    }

    webserver::WebServer& server = webserver::WebServer::getInstance(argv[1]);
    server.start();

    return (0);
}
