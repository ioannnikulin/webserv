#include "tests.hpp"

using std::cout;

int main(int argc, char** argv, char** envp) {
    (void)argc;
    (void)argv;
    (void)envp;

    runUnitTests();
    runEndToEndTests();

    cout << "All tests passed!\n";
    return (0);
}
