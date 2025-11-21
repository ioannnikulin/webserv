#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

#include "colors.hpp"

namespace output_formatting {
void printSeparator(void);
void setColor(std::string color);
void resetColor(void);
void waitForInput();
void clearTerminal();
}  // namespace output_formatting

#endif
