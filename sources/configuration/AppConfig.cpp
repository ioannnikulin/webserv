#include "AppConfig.hpp"

#include <string>
#include <utility>

#include "RouteConfig.hpp"

using std::string;

namespace webserver {
const int AppConfig::DEFAULT_MAX_BODY_SIZE = 1048576;

AppConfig::AppConfig()
    : _maxRequestBodySizeBytes(DEFAULT_MAX_BODY_SIZE) {
    _endpoints.insert(Endpoint());
}

AppConfig::AppConfig(const AppConfig& other)
    : _endpoints(other._endpoints)
    , _maxRequestBodySizeBytes(other._maxRequestBodySizeBytes)
    , _routes(other._routes) {}

AppConfig::~AppConfig() {}

AppConfig& AppConfig::addEndpoint(const Endpoint& tgt) {
    _endpoints.insert(Endpoint(tgt));
    return (*this);
}

AppConfig& AppConfig::addRoute(std::string route, const RouteConfig& tgt) {
    _routes.insert(std::make_pair(route, RouteConfig(tgt)));
    return (*this);
}

bool AppConfig::operator==(const AppConfig& other) const {
    return (
        _endpoints == other._endpoints &&
        _maxRequestBodySizeBytes == other._maxRequestBodySizeBytes && _routes == other._routes
    );
}
}  // namespace webserver
