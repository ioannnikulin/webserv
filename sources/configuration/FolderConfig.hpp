#ifndef FOLDERCONFIG_HPP
#define FOLDERCONFIG_HPP

#include <string>

namespace webserver {
class FolderConfig {
private:
    FolderConfig();
    FolderConfig(const FolderConfig& other);
    FolderConfig& operator=(const FolderConfig& other);

    std::string _rootPath;
    bool _enableListing;  // TODO 15: default false
    std::string _indexPageFileLocation;

public:
    FolderConfig(
        const std::string& rootPath,
        bool enableListing,
        const std::string& indexPageFileLocation
    );
    ~FolderConfig();
};
}  // namespace webserver

#endif
