#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>

namespace utils {
std::string separator(void);

std::string toString(int value);
std::string toString(std::size_t value);

std::string getTimestamp();

const int KIB = 1024;
const int MIB = 1024 * 1024;
const int GIB = 1024 * 1024 * 1024;

}  // namespace utils

#endif
