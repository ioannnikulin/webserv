#include "configuration/parser/ConfigChecker.hpp"

#include <cstddef>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <utility>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "configuration/FolderConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "configuration/UploadConfig.hpp"
#include "configuration/parser/ConfigParsingException.hpp"
#include "file_system/FileSystem.hpp"
#include "logger/Logger.hpp"

using std::string;

namespace {

bool isValidIpv4(const string& str) {
    if (str.empty()) {
        return (false);
    }

    std::istringstream iss(str);
    string segment;
    int segmentCount = 0;

    while (static_cast<bool>(std::getline(iss, segment, '.'))) {
        segmentCount++;

        if (segment.empty() || segment.length() > 3) {
            return (false);
        }

        for (size_t i = 0; i < segment.length(); ++i) {
            if (segment[i] < '0' || segment[i] > '9') {
                return (false);
            }
        }

        if (segment.length() > 1 && segment[0] == '0') {
            return (false);
        }

        std::istringstream segIss(segment);
        int value;
        segIss >> value;

        if (value < webserver::ConfigChecker::MIN_IPV4 ||
            value > webserver::ConfigChecker::MAX_IPV4) {
            return (false);
        }
    }

    return (segmentCount == 4);
}

bool isValidInterface(const string& str) {
    if (str.empty()) {
        return (false);
    }

    return (str == "0.0.0.0" || isValidIpv4(str));
}

}  // namespace

namespace webserver {
ConfigChecker::ConfigChecker() {
}

ConfigChecker::~ConfigChecker() {
}

void ConfigChecker::checkLocationRootIsSetOrInherit(Endpoint& endpoint) {
    Logger log;
    const std::string& serverRoot = endpoint.getRoot();
    const std::set<RouteConfig> routes = endpoint.getRoutes();

    for (std::set<RouteConfig>::iterator it = routes.begin(); it != routes.end(); ++it) {
        const RouteConfig& route = const_cast<RouteConfig&>(*it);
        FolderConfig folder = route.getFolderConfig();
        log.stream(LOG_TRACE) << "checking route " << route.getFolderConfig().getRootPath() << "\n";

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

void ConfigChecker::checkValueTypes(const Endpoint& endpoint) {
    const string& interface = endpoint.getInterface();
    if (!interface.empty() && !isValidInterface(interface)) {
        throw ConfigParsingException(
            "Invalid interface value in listen directive: '" + interface +
            "' (must be a valid IPv4 address or hostname)"
        );
    }
}

void ConfigChecker::checkNoDuplicateEndpoints(const std::set<Endpoint*>& endpoints) {
    std::set<std::pair<std::string, int> > seenBindings;

    for (std::set<Endpoint*>::const_iterator it = endpoints.begin(); it != endpoints.end(); ++it) {
        const std::string& interface = (*it)->getInterface();
        const int port = (*it)->getPort();
        const std::pair<std::string, int> binding(interface, port);

        if (seenBindings.find(binding) != seenBindings.end()) {
            std::ostringstream oss;
            oss << "Duplicate server block with listen directive '" << interface << ":" << port
                << "'";
            throw ConfigParsingException(oss.str());
        }
        seenBindings.insert(binding);
    }
}

void ConfigChecker::checkEndpoint(Endpoint& endpoint) {
    checkLocationRootIsSetOrInherit(endpoint);
    checkRootExistsOnDiskAndIsAFolder(endpoint);
    checkFilesCanBeOpened(endpoint);
    checkUploadDirectories(endpoint);
    checkValueTypes(endpoint);
}

}  // namespace webserver
