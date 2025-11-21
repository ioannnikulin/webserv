#include "WebServer.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>

#include "configuration/AppConfig.hpp"

namespace webserver {
WebServer::WebServer(const std::string& configFilePath)
    : _appConfig(0) {
    (void)configFilePath;
}

WebServer::~WebServer() {
    delete _appConfig;
}

AppConfig WebServer::getAppConfig() const {
    if (_appConfig == NULL) {
        throw std::runtime_error("Configuration uninitialized");
    }
    return (*_appConfig);
}
}  // namespace webserver
