#include "tests.hpp"

using std::cout;

int main(int argc, char **argv, char **envp) {
    (void)argc;
    (void)argv;
    (void)envp;

    unit_tests();
    e2e_tests();

    cout << "All tests passed!\n";
    return (0);
}
