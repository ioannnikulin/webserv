#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>

namespace webserver {
class Request {
private:
    std::string _type;
    std::string _location;
    std::string _protocol;
    std::map<std::string, std::string> _headers;

    static const std::string DEFAULT_TYPE;
    static const std::string DEFAULT_LOCATION;
    static const std::string DEFAULT_PROTOCOL;

public:
    Request();
    Request(const Request& other);
    explicit Request(std::string raw);
    Request& operator=(const Request& other);
    bool operator==(const Request& other) const;
    ~Request();

    Request& setType(std::string type);
    std::string getType() const;
    Request& setLocation(std::string location);
    std::string getLocation() const;
    Request& setProtocol(std::string protocol);
    Request& addHeader(std::string key, std::string value);
};
}  // namespace webserver

#endif
