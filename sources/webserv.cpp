#include <iostream>

#include "WebServer.hpp"

using std::cerr;
using std::cout;

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    cout << "Hello, World!\n";
    cerr << "This is an error message.\n";
    const webserver::WebServer serv("whatevs");

    return (0);
}
