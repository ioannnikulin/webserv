#ifndef CGIHANDLERCONFIG_HPP
#define CGIHANDLERCONFIG_HPP

#include <set>
#include <string>

namespace webserver {
class CgiHandlerConfig {
private:
    int _timeoutSeconds;
    std::string _executablePath;
    std::string _rootPath;

    // TODO 16: much more here

public:
    CgiHandlerConfig();
    CgiHandlerConfig(const CgiHandlerConfig& other);
    CgiHandlerConfig& operator=(const CgiHandlerConfig& other);

    CgiHandlerConfig(int timeoutSeconds, const std::string& executablePath);
    std::string getExtension() const;
    int getTimeoutSeconds() const;
    ~CgiHandlerConfig();

    bool operator==(const CgiHandlerConfig& other) const;
};
}  // namespace webserver

#endif
