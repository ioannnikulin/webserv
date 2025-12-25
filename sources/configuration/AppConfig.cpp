#include "AppConfig.hpp"

#include <set>
#include <string>
#include <utility>

#include "configuration/Endpoint.hpp"

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

AppConfig::~AppConfig() {
}

}  // namespace webserver
