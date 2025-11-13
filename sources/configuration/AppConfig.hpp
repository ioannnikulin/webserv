#ifndef APPCONFIG_HPP
#define APPCONFIG_HPP

#include <string>
#include <vector>

#include "Endpoint.hpp"
#include "RouteConfig.hpp"

class AppConfig {
private:
    static const int DEFAULT_MAX_BODY_SIZE;

    AppConfig(const AppConfig& other);
    AppConfig& operator=(const AppConfig& other);

    std::vector<Endpoint> _endpoints;
    int _maxRequestBodySizeBytes;
    //    std::vector<RouteConfig> _routes;

public:
    AppConfig();
    ~AppConfig();
};

#endif
