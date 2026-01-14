#ifndef CONFIGCHECKER_HPP
#define CONFIGCHECKER_HPP

#include <string>

#include "configuration/AppConfig.hpp"

namespace webserver {
class ConfigChecker {
private:
    ConfigChecker();
    ConfigChecker(const ConfigChecker& other);
    ConfigChecker& operator=(const ConfigChecker& other);

    static void checkLocationRootIsSetOrInherit(Endpoint& endpoint);
    static void checkRootExistsOnDiskAndIsAFolder(const Endpoint& endpoint);
    static void checkFilesCanBeOpened(const Endpoint& endpoint);
    static void checkCgiExecutable(const std::map<std::string, CgiHandlerConfig*>& cgiHandlers);
    static void checkUploadDirectories(const Endpoint& endpoint);

public:
    ~ConfigChecker();

    static void checkEndpoint(Endpoint& endpoint);
};
}  // namespace webserver

#endif
