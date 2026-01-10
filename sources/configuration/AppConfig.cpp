#include "AppConfig.hpp"

#include <iostream>
#include <set>
#include <stdexcept>
#include <string>

#include "configuration/Endpoint.hpp"

using std::endl;
using std::ostream;
using std::set;
using std::string;

namespace webserver {
AppConfig::AppConfig() {
}

AppConfig::AppConfig(const AppConfig& other)
    : _endpoints(other._endpoints) {
}

AppConfig& AppConfig::operator=(const AppConfig& other) {
    if (*this == other) {
        return (*this);
    }
    _endpoints = other._endpoints;
    return (*this);
}

AppConfig& AppConfig::addEndpoint(const Endpoint& tgt) {
    _endpoints.insert(Endpoint(tgt));
    return (*this);
}

bool AppConfig::operator==(const AppConfig& other) const {
    return (_endpoints == other._endpoints);
}

const std::set<Endpoint>& AppConfig::getEndpoints() const {
    return (_endpoints);
}

const Endpoint& AppConfig::getEndpoint(string interface, int port) const {
    for (set<Endpoint>::const_iterator itr = _endpoints.begin(); itr != _endpoints.end(); itr++) {
        if (itr->getInterface() == interface && itr->getPort() == port) {
            return (*itr);
        }
    }
    throw std::out_of_range("Endpoint not configured");
}

AppConfig::~AppConfig() {
}

ostream& operator<<(ostream& oss, const AppConfig& config) {
    for (set<Endpoint>::const_iterator itr = config._endpoints.begin();
         itr != config._endpoints.end();
         itr++) {
        oss << "{" << *itr << "}" << endl;
    }
    return (oss);
}

}  // namespace webserver
