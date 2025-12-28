#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace utils {
std::string separator(void);

std::string toString(int value);
std::string toString(std::size_t value);

std::string getTimestamp();

}  // namespace utils

#endif
