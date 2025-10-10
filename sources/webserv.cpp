#include "webserv.hpp"
#include <cmath>

using std::cerr;
using std::cin;
using std::cout;
using std::endl;

int main(
	int argc
	, char **argv
	, char **envp
) {
	(void)argc;
	(void)argv;
	(void)envp;

	cout << "Hello, World!" << endl;
	cerr << "This is an error message." << endl;
	cout << "The square root of 16 is " << std::sqrt(16) << endl;

	return (0);
}
