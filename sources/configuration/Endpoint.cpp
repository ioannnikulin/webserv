#include "Endpoint.hpp"

#include <string>

using std::string;

const string Endpoint::DEFAULT_INTERFACE = "127.0.0.1";
const int Endpoint::DEFAULT_PORT = 888;

Endpoint::Endpoint()
    : _interface(DEFAULT_INTERFACE)
    , _port(DEFAULT_PORT) {}

Endpoint::Endpoint(const std::string& interface, int port)
    : _interface(interface)
    , _port(port) {}

Endpoint::Endpoint(const Endpoint& other)
    : _interface(other._interface)
    , _port(other._port) {}

Endpoint& Endpoint::operator=(const Endpoint& other) {
    (void)other;
    return (*this);
}

Endpoint::~Endpoint() {}
