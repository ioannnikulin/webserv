#include "configuration/parser/ConfigParsingException.hpp"

#include <sstream>
#include <string>

using std::ostringstream;
using std::string;

namespace webserver {
const char* ConfigParsingException::what() const throw() {
    return (_message.c_str());
}

ConfigParsingException::ConfigParsingException(std::string message)
    : _message(message) {
}

ConfigParsingException& ConfigParsingException::operator=(const ConfigParsingException& other) {
    if (this != &other) {
        _message = other._message;
    }
    return (*this);
}

ConfigParsingException::ConfigParsingException(const ConfigParsingException& other) {
    if (this == &other) {
        return;
    }
    _message = other._message;
}

ConfigParsingException::~ConfigParsingException() throw() {
}

}  // namespace webserver
