#ifndef APPCONFIG_HPP
#define APPCONFIG_HPP

#include <set>
#include <string>

#include "Endpoint.hpp"
#include "RouteConfig.hpp"

namespace webserver {
class AppConfig {
private:
    static const int DEFAULT_MAX_BODY_SIZE;

    AppConfig& operator=(const AppConfig& other);

    std::set<Endpoint> _endpoints;
    int _maxRequestBodySizeBytes;
    std::map<std::string, RouteConfig> _routes;  // NOTE: route as key (e.g. /)

public:
    AppConfig();
    AppConfig(const AppConfig& other);
    ~AppConfig();

    std::set<std::pair<std::string, int> > getAllInterfacePortPairs(void) const;
    AppConfig& addEndpoint(const Endpoint& tgt);
    AppConfig& addRoute(std::string route, const RouteConfig& tgt);

    const RouteConfig& getRoute(std::string route) const;

    bool operator==(const AppConfig& other) const;
};
}  // namespace webserver

#endif
