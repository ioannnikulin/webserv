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
    std::string _body;

    static const std::string DEFAULT_TYPE;
    static const std::string DEFAULT_LOCATION;
    static const std::string DEFAULT_PROTOCOL;

    static const std::string MALFORMED_FIRST_LINE;

    void parseFirstLine(std::string firstLine);
    void parseHeaders(std::string rawHeaders);
    void parseBody(std::string body);

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
    std::string getHeader(std::string key) const;
};
}  // namespace webserver

#endif
