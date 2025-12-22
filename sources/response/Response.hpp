#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <map>
#include <string>

namespace webserver {

class Response {
private:
    int _statusCode;
    std::map<std::string, std::string> _headers;
    std::string _body;
    Response();

public:
    Response(int status, const std::string& body, const std::string& type);
    Response& operator=(const Response& other);
    Response(const Response& other);
    ~Response();

    std::string serialize() const;

    int getStatus() const;
    const std::string& getBody() const;
    std::string getHeader(const std::string& key) const;

    void setStatus(int status);
    void setBody(std::string fileContent);
    void setHeader(const std::string& key, const std::string& value);
};
}  // namespace webserver
#endif
