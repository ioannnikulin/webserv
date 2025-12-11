#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>

namespace webserver {
class Request {
private:
    std::string _method;  // NOTE: GET, POST, DELETE
    std::string
        _requestTarget;  // NOTE: request-target is the official name as per RFC 7230; _location is a bad name
                         // NOTE: request-target is the full form as in /foo/bar?x=1
    std::string _path;   // NOTE: only /foo/bar
    std::string _query;  // NOTE: only x=1
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

    std::string getType() const;
    Request& setType(std::string type);

    std::string getRequestTarget() const;
    Request& setRequestTarget(std::string location);

    std::string getPath() const;

    std::string getQuery() const;

    std::string getBody() const;

    std::string getProtocol() const;
    Request& setProtocol(std::string protocol);

    Request& addHeader(std::string key, std::string value);
    std::string getHeader(std::string key) const;
};
}  // namespace webserver

#endif
