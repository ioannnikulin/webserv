#include "utils.hpp"

#include <iostream>
#include <string>

#include "colors.hpp"

#define SEPARATOR_WIDTH 80
#define SEPARATOR_CHAR '='

void setColor(std::string color) {
    std::cout << color;
}

void resetColor(void) {
    std::cout << RESET;
}

void printSeparator(void) {
    setColor(CYAN);
    std::cout << std::string(SEPARATOR_WIDTH, SEPARATOR_CHAR);
    resetColor();
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
