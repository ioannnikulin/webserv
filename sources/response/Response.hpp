#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>

#include "logger/Logger.hpp"

namespace webserver {

class Response {
private:
    static Logger _log;
    int _statusCode;
    std::map<std::string, std::string> _headers;
    std::string _body;

public:
    Response();
    Response(int status, const std::string& body, const std::string& type);
    Response& operator=(const Response& other);
    Response(const Response& other);
    ~Response();

    std::string serialize() const;

    int getStatus() const;
    const std::string& getBody() const;
    std::string getHeader(const std::string& key) const;

    Response& setStatus(int status);
    Response& setBody(std::string fileContent);
    Response& setHeader(const std::string& key, const std::string& value);
};
}  // namespace webserver
#endif
