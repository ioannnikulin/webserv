#include "AppConfig.hpp"

#include <string>

using std::string;

namespace webserver {
const string AppConfig::_defaultInterface = "0.0.0.0";

AppConfig::AppConfig(string filePath)
    : _interface(_defaultInterface)
    , _port(_defaultPort) {
    (void)filePath;
}

AppConfig::~AppConfig() {}

string AppConfig::getInterface() const {
    return _interface;
}

int AppConfig::getPort() const {
    return _port;
}
}  // namespace webserver