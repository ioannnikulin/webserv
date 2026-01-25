#include "AppConfig.hpp"

#include <iostream>
#include <set>
#include <stdexcept>
#include <string>

#include "configuration/Endpoint.hpp"

using std::ostream;
using std::set;
using std::string;

namespace webserver {
AppConfig::AppConfig() {
}

AppConfig::AppConfig(const AppConfig& other) {
    for (set<Endpoint*>::const_iterator itr = other._endpoints.begin();
         itr != other._endpoints.end();
         itr++) {
        _endpoints.insert(new Endpoint(**itr));
    }
}

AppConfig& AppConfig::operator=(const AppConfig& other) {
    if (this == &other) {
        return (*this);
    }
    for (set<Endpoint*>::iterator itr = _endpoints.begin(); itr != _endpoints.end(); itr++) {
        delete *itr;
    }
    for (set<Endpoint*>::const_iterator itr = other._endpoints.begin();
         itr != other._endpoints.end();
         itr++) {
        _endpoints.insert(new Endpoint(**itr));
    }
    return (*this);
}

AppConfig& AppConfig::addEndpoint(const Endpoint& tgt) {
    _endpoints.insert(new Endpoint(tgt));
    return (*this);
}

bool AppConfig::operator==(const AppConfig& other) const {
    if (_endpoints.size() != other._endpoints.size()) {
        return (false);
    }
    set<Endpoint*>::const_iterator itr = _endpoints.begin();
    set<Endpoint*>::const_iterator itro = other._endpoints.begin();
    for (; itr != _endpoints.end(); itr++, itro++) {
        if (!((**itr) == (**itro))) {
            return (false);
        }
    }
    return (true);
}

const std::set<Endpoint*>& AppConfig::getEndpoints() const {
    return (_endpoints);
}

const Endpoint* AppConfig::getEndpoint(string interface, int port) const {
    for (set<Endpoint*>::const_iterator itr = _endpoints.begin(); itr != _endpoints.end(); itr++) {
        if ((*itr)->getInterface() == interface && (*itr)->getPort() == port) {
            return (*itr);
        }
    }
    throw std::out_of_range("Endpoint not configured");
}

AppConfig::~AppConfig() {
    for (set<Endpoint*>::iterator itr = _endpoints.begin(); itr != _endpoints.end(); itr++) {
        delete *itr;
    }
}

ostream& operator<<(ostream& oss, const AppConfig& config) {
    for (set<Endpoint*>::const_iterator itr = config._endpoints.begin();
         itr != config._endpoints.end();
         itr++) {
        oss << "{" << **itr << "}\n";
    }
    return (oss);
}

}  // namespace webserver
