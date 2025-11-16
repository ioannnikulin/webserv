#include "AppConfig.hpp"

#include <string>

using std::string;

namespace webserver {
const int AppConfig::DEFAULT_MAX_BODY_SIZE = 1048576;

AppConfig::AppConfig()
    : _maxRequestBodySizeBytes(DEFAULT_MAX_BODY_SIZE) {
    _endpoints.push_back(Endpoint());
}
}  // namespace webserver
