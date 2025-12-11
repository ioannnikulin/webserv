#ifndef APPCONFIG_HPP
#define APPCONFIG_HPP

#include <set>
#include <string>

#include "configuration/Endpoint.hpp"
#include "configuration/RouteConfig.hpp"

namespace webserver {
class AppConfig {
private:
    AppConfig& operator=(const AppConfig& other);

    std::set<Endpoint> _endpoints;

public:
    AppConfig();
    AppConfig(const AppConfig& other);
    ~AppConfig();

    std::set<std::pair<std::string, int> > getAllInterfacePortPairs(void) const;
    AppConfig& addEndpoint(const Endpoint& tgt);
    std::set<Endpoint> getEndpoints() const;

    const RouteConfig& getRoute(std::string route) const;

    bool operator==(const AppConfig& other) const;
};
}  // namespace webserver

#endif
