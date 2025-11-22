#include "CgiHandlerConfig.hpp"

namespace webserver {
CgiHandlerConfig::CgiHandlerConfig(const CgiHandlerConfig& other)
    : _executablePath(other._executablePath)
    , _rootPath(other._rootPath)
    , _timeoutSeconds(other._timeoutSeconds) {}

bool CgiHandlerConfig::operator==(const CgiHandlerConfig& other) const {
    return (
        _executablePath == other._executablePath &&
        _rootPath == other._rootPath && _timeoutSeconds == other._timeoutSeconds
    );
}

CgiHandlerConfig::~CgiHandlerConfig() {}
}  // namespace webserver
