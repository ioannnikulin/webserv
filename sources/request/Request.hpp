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
    bool _isRequestTargetReceived;
    // NOTE: technically request can be split even here, so yes, this should be checked separately
    std::string _path;   // NOTE: only /foo/bar
    std::string _query;  // NOTE: only x=1
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
    // NOTE: not const due to lazy body initalization
    bool operator==(const Request& other);
    ~Request();

    HttpMethodType getType() const;
    Request& setType(HttpMethodType type);

    std::string getRequestTarget() const;
    bool isRequestTargetReceived() const;
    Request& setRequestTarget(std::string requestTarget);

    std::string getPath() const;
    Request& setPath(std::string path);

    std::string getQuery() const;

    // NOTE: not const due to lazy body initalization
    std::string getBody();
    Request& setBody(std::string body);
    Request& setIsBodyRaw(bool isBodyRaw);

    std::string getVersion() const;
    Request& setVersion(std::string version);

    Request& addHeader(std::string key, std::string value);
    std::string getHeader(std::string key) const;
    bool contentLengthSet() const;
    size_t getContentLength() const;
    void setMaxClientBodySizeBytes(size_t maxClientBodySizeBytes);
    size_t getMaxClientBodySizeBytes() const;
    static size_t defaultMaxClientBodySizeBytes();
    friend std::ostream& operator<<(std::ostream& oss, const Request& request);
};

}  // namespace webserver

#endif
