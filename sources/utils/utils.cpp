#include "utils.hpp"

#include <cstddef>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

#include "colors.hpp"

#define SEPARATOR_WIDTH 80
#define SEPARATOR_CHAR '='
#define SEPARATOR_COLOR CYAN

using std::ostringstream;
using std::string;

namespace utils {
string separator(void) {
    ostringstream oss;
    oss << SEPARATOR_COLOR << string(SEPARATOR_WIDTH, SEPARATOR_CHAR) << RESET_COLOR;
    return (oss.str());
}

string toString(int value) {
    ostringstream oss;
    oss << value;
    return (oss.str());
}

string toString(std::size_t value) {
    ostringstream oss;
    oss << value;
    return (oss.str());
}

string getTimestamp() {
    const std::time_t now = std::time(0);
    const std::tm gmt = *std::gmtime(&now);
    const int BUFSIZE = 64;
    char buf[BUFSIZE];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &gmt);
    return (string(buf));
}

}  // namespace utils
