#include "CgiHandlerConfig.hpp"

#include <iostream>
#include <string>

using std::ostream;

using std::string;
namespace webserver {
CgiHandlerConfig::CgiHandlerConfig()
    : _timeoutSeconds(0)
    , _executablePath("")
    , _storageRootPath("") {
}

CgiHandlerConfig::CgiHandlerConfig(const CgiHandlerConfig& other)
    : _timeoutSeconds(other._timeoutSeconds)
    , _executablePath(other._executablePath)
    , _storageRootPath(other._storageRootPath) {
}

CgiHandlerConfig& CgiHandlerConfig::operator=(const CgiHandlerConfig& other) {
    if (this == &other) {
        return (*this);
    }
    _executablePath = other._executablePath;
    _storageRootPath = other._storageRootPath;
    _timeoutSeconds = other._timeoutSeconds;
    return (*this);
}

CgiHandlerConfig::CgiHandlerConfig(int timeoutSeconds, const string& executablePath)
    : _timeoutSeconds(timeoutSeconds)
    , _executablePath(executablePath)
    , _storageRootPath("") {
}

std::string CgiHandlerConfig::getExecutablePath() const {
    return (_executablePath);
}

bool CgiHandlerConfig::operator==(const CgiHandlerConfig& other) const {
    return (
        _executablePath == other._executablePath && _storageRootPath == other._storageRootPath &&
        _timeoutSeconds == other._timeoutSeconds
    );
}

CgiHandlerConfig::~CgiHandlerConfig() {
}

ostream& operator<<(ostream& oss, const CgiHandlerConfig& cgi) {
    oss << cgi._timeoutSeconds;
    oss << " " << cgi._executablePath;
    oss << " " << cgi._storageRootPath;
    oss << "\n";
    return (oss);
}

}  // namespace webserver
