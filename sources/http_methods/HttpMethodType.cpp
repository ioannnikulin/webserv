#include "HttpMethodType.hpp"

#include <stdexcept>
#include <string>

using std::runtime_error;
using std::string;

namespace webserver {
HttpMethodType stringToMethod(const string& str) {
    if (str == "GET") {
        return (GET);
    }
    if (str == "POST") {
        return (POST);
    }
    if (str == "DELETE") {
        return (DELETE);
    }

    throw runtime_error("Invalid HTTP method in limit_except: " + str);
}
}  // namespace webserver
