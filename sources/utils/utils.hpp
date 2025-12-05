#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace utils {
void printSeparator(void);
void setColor(std::string color);
void resetColor(void);

std::string toString(int x);
void waitForInput();
void clearTerminal();
}  // namespace utils

#endif
