#include "AppConfig.hpp"

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>

#include "configuration/Endpoint.hpp"

using std::map;
using std::pair;
using std::set;
using std::string;

namespace webserver {

AppConfig::AppConfig() {
}

AppConfig::AppConfig(const AppConfig& other)
    : _endpoints(other._endpoints) {
}

AppConfig& AppConfig::addEndpoint(const Endpoint& tgt) {
    _endpoints.insert(Endpoint(tgt));
    return (*this);
}

bool AppConfig::operator==(const AppConfig& other) const {
    return (_endpoints == other._endpoints);
}

std::set<Endpoint> AppConfig::getEndpoints() const {
    return (_endpoints);
}

set<pair<string, int> > AppConfig::getAllInterfacePortPairs(void) const {
    set<pair<string, int> > result;

    for (set<Endpoint>::const_iterator itr = _endpoints.begin(); itr != _endpoints.end(); itr++) {
        result.insert(pair<string, int>(itr->getInterface(), itr->getPort()));
    }
    return (result);
}

const RouteConfig& AppConfig::getRoute(std::string route) const {
    const map<string, RouteConfig>::const_iterator itr = _routes.find(route);

    if (itr == _routes.end()) {
        throw std::runtime_error("Route not found: " + route);
    }
    return itr->second;
}

AppConfig::~AppConfig() {
}

}  // namespace webserver
