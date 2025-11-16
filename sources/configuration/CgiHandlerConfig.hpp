#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <string>

namespace webserver {
class CgiHandlerConfig {
private:
    CgiHandlerConfig();
    CgiHandlerConfig(const CgiHandlerConfig& other);
    CgiHandlerConfig& operator=(const CgiHandlerConfig& other);

    std::string _executablePath;
    std::string _rootPath;
    int _timeoutSeconds;
    // TODO 16: much more here

public:
    CgiHandlerConfig(
        const std::string& rootPath,
        bool enableListing,
        const std::string& indexPageFileLocation
    );
    ~CgiHandlerConfig();
};
}  // namespace webserver

#endif
