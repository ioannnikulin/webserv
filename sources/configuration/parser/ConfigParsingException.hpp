#ifndef CONFIGPARSINGEXCEPTION_HPP
#define CONFIGPARSINGEXCEPTION_HPP

#include <exception>
#include <string>

namespace webserver {
class ConfigParsingException : public std::exception {
private:
    std::string _message;

protected:
    ConfigParsingException& operator=(const ConfigParsingException& other);

public:
    ConfigParsingException(std::string message);
    ConfigParsingException(const ConfigParsingException& other);
    virtual ~ConfigParsingException() throw();
    virtual const char* what() const throw();
};
}  // namespace webserver
#endif
