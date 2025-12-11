#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace utils {
void printSeparator(void);
void setColor(std::string color);
void resetColor(void);

std::string toString(int value);
std::string toString(std::size_t value);

std::string getTimestamp();

void waitForInput();
void clearTerminal();
}  // namespace utils

#endif
