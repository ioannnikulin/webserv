#include "utils.hpp"

#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>

#include "colors.hpp"

#define SEPARATOR_WIDTH 80
#define SEPARATOR_CHAR '='
#define SEPARATOR_COLOR CYAN

namespace utils {
std::string separator(void) {
    std::ostringstream oss;
    oss << SEPARATOR_COLOR << std::string(SEPARATOR_WIDTH, SEPARATOR_CHAR) << RESET_COLOR
        << std::endl;
    return (oss.str());
}

std::string toString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string toString(std::size_t value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string getTimestamp() {
    /* NOTE: 
    #include <ctime>
    #include <iomanip>
    std::time_t now = std::time(0);
    std::tm gmt = *std::gmtime(&now);
    char buf[64];
    Format: "Mon, 10 Oct 2025 10:00:00 GMT"
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &gmt);
    return std::string(buf);
    */
    return ("[BEAUTIFUL TIMESTAMP THAT USES A FORBIDDEN FUNCTION]");
}

}  // namespace utils
