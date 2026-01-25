#ifndef APPCONFIG_HPP
#define APPCONFIG_HPP

#include <set>
#include <string>

#include "configuration/Endpoint.hpp"
#include "configuration/RouteConfig.hpp"

namespace webserver {
class AppConfig {
private:
    /* NOTE: if we store this as a set of Endpoints, when the set is relocated, all pointers break
    * and we do have pointers to http status catalogues down in other objects, for example,
    * so storing as set of pointers to Endpoints
    */
    std::set<Endpoint*> _endpoints;

public:
    AppConfig();
    AppConfig(const AppConfig& other);
    AppConfig& operator=(const AppConfig& other);
    ~AppConfig();

    AppConfig& addEndpoint(const Endpoint& tgt);
    const std::set<Endpoint*>& getEndpoints() const;
    const Endpoint* getEndpoint(std::string interface, int port) const;

    bool operator==(const AppConfig& other) const;
    friend std::ostream& operator<<(std::ostream& oss, const AppConfig& config);
};
}  // namespace webserver

#endif
