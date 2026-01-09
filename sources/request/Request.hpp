#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <map>
#include <string>

#include "http_methods/HttpMethodType.hpp"

namespace webserver {
class Request {
private:
    HttpMethodType _method;
    std::string _requestTarget;  // NOTE: full form as in /foo/bar?x=1
    std::string _path;           // NOTE: only /foo/bar
    std::string _query;          // NOTE: only x=1
    std::string _protocolVersion;
    std::map<std::string, std::string> _headers;
    bool _isBodyRaw;
    std::string _body;

    static const HttpMethodType DEFAULT_TYPE;
    static const std::string DEFAULT_REQUEST_TARGET;
    static const std::string DEFAULT_HTTP_VERSION;

    static const std::string MALFORMED_FIRST_LINE;

    size_t _maxClientBodySizeBytes;

    void parseFirstLine(const std::string& firstLine);
    void parseHeaders(const std::string& rawHeaders);
    void parseChunkedBody();
    void parseBody();

public:
    Request();
    Request(const Request& other);
    explicit Request(std::string raw);
    Request& operator=(const Request& other);
    bool operator==(const Request& other) const;
    ~Request();

    HttpMethodType getType() const;
    Request& setType(HttpMethodType type);

    std::string getRequestTarget() const;
    Request& setRequestTarget(std::string requestTarget);

    std::string getPath() const;

    std::string getQuery() const;

    std::string getBody();
    Request& setBody(std::string body);

    std::string getVersion() const;
    Request& setVersion(std::string version);

    Request& addHeader(std::string key, std::string value);
    std::string getHeader(std::string key) const;
    bool contentLengthSet() const;
    size_t getContentLength() const;
    void setMaxBodySizeBytes(size_t maxBodySizeBytes);
    static size_t DEFAULT_MAX_CLIENT_BODY_SIZE_BYTES;
};
}  // namespace webserver

#endif
