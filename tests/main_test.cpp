#include "tests.hpp"

int main(int argc, char **argv, char **envp) {
	(void)argc;
	(void)argv;
	(void)envp;

	unit_tests();
	e2e_tests();

	return (0);
}
