#include "utils.hpp"

#include <iostream>
#include <string>

#include "colors.hpp"

#define SEPARATOR_WIDTH 80
#define SEPARATOR_CHAR '='
#define SEPARATOR_COLOR CYAN

void output_formatting::setColor(std::string color) {
    std::cout << color;
}

void output_formatting::resetColor(void) {
    std::cout << RESET;
}

void output_formatting::printSeparator(void) {
    output_formatting::setColor(SEPARATOR_COLOR);
    std::cout << std::string(SEPARATOR_WIDTH, SEPARATOR_CHAR);
    output_formatting::resetColor();
    std::cout << std::endl;
}

// void waitForInput() {
//     std::string input;
//     std::cout << "Press ENTER to continue...";
//     std::getline(std::cin, input);
// }

// void clearTerminal() {
//     std::cout << "\033[2J\033[H";
// }
