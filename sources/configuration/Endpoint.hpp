#ifndef ENDPOINT_HPP
#define ENDPOINT_HPP

#include <string>

namespace webserver {
class Endpoint {
private:
    static const std::string DEFAULT_INTERFACE;
    static const int DEFAULT_PORT;

    const std::string _interface;
    const int _port;

public:
    Endpoint();
    Endpoint(const std::string& interface, int port);
    Endpoint(const Endpoint& other);
    Endpoint& operator=(const Endpoint& other);
    bool operator<(const Endpoint& other) const;
    bool operator==(const Endpoint& other) const;
    ~Endpoint();
};
}  // namespace webserver

#endif
