#ifndef APPCONFIG_HPP
#define APPCONFIG_HPP

#include <set>
#include <string>

#include "configuration/Endpoint.hpp"
#include "configuration/RouteConfig.hpp"

namespace webserver {
class AppConfig {
private:
    std::set<Endpoint> _endpoints;

public:
    AppConfig();
    AppConfig(const AppConfig& other);
    AppConfig& operator=(const AppConfig& other);
    ~AppConfig();

    AppConfig& addEndpoint(const Endpoint& tgt);
    const std::set<Endpoint>& getEndpoints() const;
    const Endpoint& getEndpoint(std::string interface, int port) const;

    bool operator==(const AppConfig& other) const;
    friend std::ostream& operator<<(std::ostream& oss, const AppConfig& config);
};
}  // namespace webserver

#endif
