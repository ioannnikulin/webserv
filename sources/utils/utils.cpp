#include "utils.hpp"

#include <iostream>
#include <sstream>
#include <string>

#include "colors.hpp"

#define SEPARATOR_WIDTH 80
#define SEPARATOR_CHAR '='
#define SEPARATOR_COLOR CYAN

namespace utils {
void setColor(std::string color) {
    std::cout << color;
}

void resetColor(void) {
    std::cout << RESET;
}

void printSeparator(void) {
    setColor(SEPARATOR_COLOR);
    std::cout << std::string(SEPARATOR_WIDTH, SEPARATOR_CHAR);
    resetColor();
    std::cout << std::endl;
}

std::string toString(int x) {
    std::ostringstream oss;
    oss << x;
    return oss.str();
}
}  // namespace utils

/* NOTE: unused functions
void waitForInput() {
	std::string input;
	std::cout << "Press ENTER to continue...";
	std::getline(std::cin, input);
	}

void clearTerminal() {
	std::cout << "\033[2J\033[H";
	}
*/
