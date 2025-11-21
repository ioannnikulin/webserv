#include "CgiHandlerConfig.hpp"

namespace webserver {
CgiHandlerConfig::CgiHandlerConfig(const CgiHandlerConfig& other)
    : _extensions(other._extensions)
    , _executablePath(other._executablePath)
    , _rootPath(other._rootPath)
    , _timeoutSeconds(other._timeoutSeconds) {}

bool CgiHandlerConfig::operator==(const CgiHandlerConfig& other) const {
    return (
        _extensions == other._extensions && _executablePath == other._executablePath &&
        _rootPath == other._rootPath && _timeoutSeconds == other._timeoutSeconds
    );
}

bool CgiHandlerConfig::operator<(const CgiHandlerConfig& other) const {
    return (_extensions < other._extensions);
}

CgiHandlerConfig::~CgiHandlerConfig() {}
}  // namespace webserver
