#ifndef ENDPOINT_HPP
#define ENDPOINT_HPP

#include <map>
#include <string>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "configuration/UploadConfig.hpp"

namespace webserver {
class Endpoint {
private:
    std::string _interface;
    int _port;
    std::string _serverName;
    std::string _rootDirectory;
    size_t _maxClientBodySizeBytes;
    std::map<std::string, CgiHandlerConfig*> _cgiHandlers;
    std::set<RouteConfig> _routes;
    const UploadConfig* _uploadConfig;

    static const int MIN_PORT = 1;
    static const int MAX_PORT = 65535;

public:
    static const std::string DEFAULT_INTERFACE;
    static const int DEFAULT_PORT;
    static const unsigned long DEFAULT_MAX_CLIENT_BODY_SIZE_BYTES;
    static const std::string DEFAULT_ROOT;

    Endpoint();
    Endpoint(const std::string& interface, int port);
    Endpoint(const Endpoint& other);
    Endpoint& operator=(const Endpoint& other);
    // NOTE: DL: operator< is REQUIRED for storing in std::set
    bool operator<(const Endpoint& other) const;
    bool operator==(const Endpoint& other) const;
    ~Endpoint();

    Endpoint& setInterface(std::string interface);
    Endpoint& setPort(const int& port);
    Endpoint& setRoot(const std::string& path);
    Endpoint& setMaxClientBodySizeBytes(size_t size);
    size_t getMaxClientBodySizeBytes() const;
    Endpoint& addServerName(const std::string& name);
    Endpoint& addCgiHandler(const CgiHandlerConfig& config, std::string extension);
    Endpoint& addRoute(const RouteConfig& route);
    Endpoint& setUploadConfig(const UploadConfig& cfg);
    std::string getInterface() const;
    int getPort() const;
    std::string getRoot() const;
    RouteConfig getRoute(std::string route) const;
    RouteConfig selectRoute(std::string route) const;
    std::set<RouteConfig> getRoutes() const;

    static bool isAValidPort(int port);
};
}  // namespace webserver

#endif
