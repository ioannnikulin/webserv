#ifndef ENDPOINT_HPP
#define ENDPOINT_HPP

#include <map>
#include <string>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "configuration/UploadConfig.hpp"
#include "http_status/HttpStatus.hpp"

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
    HttpStatus _statusCatalogue;

    static const int MIN_PORT = 1;
    static const int MAX_PORT = 65535;

public:
    static const std::string DEFAULT_INTERFACE;
    static const int DEFAULT_PORT;
    static size_t defaultMaxClientBodySizeBytes();
    static const std::string DEFAULT_ROOT;

    Endpoint();
    Endpoint(const std::string& interface, int port);
    Endpoint(const Endpoint& other);
    Endpoint& operator=(const Endpoint& other);
    // NOTE: DL: operator< is REQUIRED for storing in std::set
    bool operator<(const Endpoint& other) const;
    bool operator==(const Endpoint& other) const;
    ~Endpoint();

    const HttpStatus& getStatusCatalogue() const;
    Endpoint& setStatusCatalogue(const HttpStatus& statusCatalogue);
    Endpoint& setStatusPage(int code, const std::string& pageFileLocation);
    Endpoint& setInterface(std::string interface);
    Endpoint& setPort(const int& port);
    Endpoint& setRoot(const std::string& path);
    Endpoint& setMaxClientBodySizeBytes(size_t size);
    size_t getMaxClientBodySizeBytes() const;
    Endpoint& addServerName(const std::string& name);
    Endpoint& addCgiHandler(const CgiHandlerConfig& config, std::string extension);
    Endpoint& addRoute(RouteConfig route);
    std::string getInterface() const;
    std::string getRoot() const;
    std::string getServerName() const;
    int getPort() const;
    const RouteConfig& getRoute(std::string route) const;
    const RouteConfig& selectRoute(std::string route) const;
    const std::set<RouteConfig>& getRoutes() const;
    const std::map<std::string, CgiHandlerConfig*>& getCgiHandlers() const;

    static bool isAValidPort(int port);
    friend std::ostream& operator<<(std::ostream& oss, const Endpoint& endpoint);
};
}  // namespace webserver

#endif
