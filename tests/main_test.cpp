#include <iostream>

#include "tests.hpp"

using std::cout;

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    webserver::runUnitTests();
    webserver::runEndToEndTests();

    cout << "All tests passed!\n";
    return (0);
}
