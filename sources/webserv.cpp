#include <iostream>
#include <string>

#include "WebServer.hpp"
#include "utils/colors.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << B_RED << "Failed to launch: no config file provided. Usage: " << argv[0]
                  << " <config_file>\n"
                  << RESET_COLOR << "Example: ./webserv tests/config_files/local_run.conf\n";
        return (1);
    }
    webserver::WebServer& server = webserver::WebServer::getInstance(argv[1]);
    server.start();

    return (0);
}
