#include "configuration/parser/ConfigChecker.hpp"

#include <map>
#include <set>
#include <string>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "configuration/FolderConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "configuration/UploadConfig.hpp"
#include "configuration/parser/ConfigParsingException.hpp"
#include "file_system/FileSystem.hpp"

using std::string;

namespace webserver {
ConfigChecker::ConfigChecker() {
}

ConfigChecker::~ConfigChecker() {
}

void ConfigChecker::checkLocationRootIsSetOrInherit(Endpoint& endpoint) {
    const std::string& serverRoot = endpoint.getRoot();
    const std::set<RouteConfig> routes = endpoint.getRoutes();

    for (std::set<RouteConfig>::iterator it = routes.begin(); it != routes.end(); ++it) {
        const RouteConfig& route = const_cast<RouteConfig&>(*it);
        FolderConfig folder = route.getFolderConfig();

        if (folder.doesLocationBlockServeFiles()) {
            const std::string& locationRoot = folder.getRootPath();

            if (locationRoot.empty()) {
                if (serverRoot.empty()) {
                    throw ConfigParsingException(
                        "Location '" + route.getPath() +
                        "' has no root directive and server block has no root directive"
                    );
                }
                folder.setRootPath(serverRoot);
            }
        }
    }
}

void ConfigChecker::checkRootExistsOnDiskAndIsAFolder(const Endpoint& endpoint) {
    const std::set<RouteConfig> routes = endpoint.getRoutes();

    const std::string server_root = endpoint.getRoot();

    if (!server_root.empty() && !file_system::isDirectory(server_root.c_str())) {
        throw ConfigParsingException(
            "Root path '" + server_root + "' does not exist or is not a directory"
        );
    }
    for (std::set<RouteConfig>::const_iterator it = routes.begin(); it != routes.end(); ++it) {
        if (it->getFolderConfig().doesLocationBlockServeFiles()) {
            const string path = it->getFolderConfig().getRootPath();
            if (!file_system::isDirectory(path.c_str())) {
                throw ConfigParsingException(
                    "Root path '" + path + "' for location '" + it->getPath() +
                    "' does not exist or is not a directory"
                );
            }
        }
    }
}

void ConfigChecker::checkCgiExecutable(const std::map<std::string, CgiHandlerConfig*>& cgiHandlers
) {
    for (std::map<std::string, CgiHandlerConfig*>::const_iterator it = cgiHandlers.begin();
         it != cgiHandlers.end();
         ++it) {
        const string& execPath = it->second->getExecutablePath();

        if (!file_system::isExecutableFile(execPath.c_str())) {
            throw ConfigParsingException(
                "CGI executable '" + execPath + "' for extension '" + it->first +
                "' does not exist or is not executable"
            );
        }
    }
}

void ConfigChecker::checkFilesCanBeOpened(const Endpoint& endpoint) {
    const std::set<RouteConfig> routes = endpoint.getRoutes();

    for (std::set<RouteConfig>::const_iterator it = routes.begin(); it != routes.end(); ++it) {
        const RouteConfig& route = *it;
        const FolderConfig& folder = route.getFolderConfig();

        if (!folder.getIndexPageFilename().empty()) {
            const string indexPath = folder.getRootPath() + "/" + folder.getIndexPageFilename();

            if (!file_system::isReadableFile(indexPath.c_str())) {
                throw ConfigParsingException(
                    "Cannot open index file '" + indexPath + "' for location '" + route.getPath() +
                    "': file does not exist or is not readable"
                );
            }
        }
        const std::map<std::string, CgiHandlerConfig*>& locationCgiHandlers =
            route.getCgiHandlers();
        checkCgiExecutable(locationCgiHandlers);
    }

    const std::map<std::string, CgiHandlerConfig*>& cgiHandlers = endpoint.getCgiHandlers();
    checkCgiExecutable(cgiHandlers);
}

void ConfigChecker::checkUploadDirectories(const Endpoint& endpoint) {
    const std::set<RouteConfig> routes = endpoint.getRoutes();
    for (std::set<RouteConfig>::const_iterator it = routes.begin(); it != routes.end(); ++it) {
        const UploadConfig& locUpload = it->getUploadConfigSection();
        if (locUpload.isUploadEnabled()) {
            const string& uploadPath = locUpload.getUploadRootFolder();
            if (!file_system::isWritableDirectory(uploadPath.c_str())) {
                throw ConfigParsingException(
                    "Upload directory '" + uploadPath + "' for location '" + it->getPath() +
                    "' does not exist or is not writable"
                );
            }
        }
    }
}

void ConfigChecker::checkEndpoint(Endpoint& endpoint) {
    checkLocationRootIsSetOrInherit(endpoint);
    checkRootExistsOnDiskAndIsAFolder(endpoint);
    checkFilesCanBeOpened(endpoint);
    checkUploadDirectories(endpoint);
}

}  // namespace webserver
