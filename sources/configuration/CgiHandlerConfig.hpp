#ifndef CGIHANDLER_HPP
#define CGIHANDLER_HPP

#include <set>
#include <string>

namespace webserver {
class CgiHandlerConfig {
private:
    CgiHandlerConfig();
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
    CgiHandlerConfig(const CgiHandlerConfig& other);
    ~CgiHandlerConfig();

    bool operator==(const CgiHandlerConfig& other) const;
};
}  // namespace webserver

#endif
