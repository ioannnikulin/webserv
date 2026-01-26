#include "http_methods/HttpMethodType.hpp"

#include <stdexcept>
#include <string>

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
    if (str == "SHUTDOWN") {
        return (SHUTDOWN);
    }

    throw std::out_of_range("Invalid HTTP method: " + str);
}

std::string methodToString(const HttpMethodType& method) {
    switch (method) {
        case GET: {
            return ("GET");
        }
        case POST: {
            return ("POST");
        }
        case DELETE: {
            return ("DELETE");
        }
        case SHUTDOWN: {
            return ("SHUTDOWN");
        }
        default: {
            throw std::out_of_range("Invalid HTTP method");
        }
    }
}
}  // namespace webserver
