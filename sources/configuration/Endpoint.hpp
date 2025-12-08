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
    size_t _maxRequestBodySizeBytes;
    std::map<std::string, CgiHandlerConfig> _cgiHandlers;
    std::vector<RouteConfig> _routes;
    UploadConfig* _uploadConfig;

    static const int MIN_PORT = 1;
    static const int MAX_PORT = 65535;

public:
    static const std::string DEFAULT_INTERFACE;
    static const int DEFAULT_PORT;
    static const size_t DEFAULT_MAX_BODY_SIZE;
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
    void setPort(const int& port);
    void setRoot(const std::string& path);
    void setClientMaxBodySize(size_t size);
    Endpoint& addServerName(const std::string& name);
    void addCgiHandler(const CgiHandlerConfig& config, std::string extension);
    void addRoute(const RouteConfig& route);
    void setUploadConfig(const UploadConfig& cfg);
    std::string getInterface() const;
    int getPort() const;
    std::vector<RouteConfig> getRoutes() const;

    static bool isAValidPort(int port);
};
}  // namespace webserver

#endif
