#ifndef FOLDERCONFIG_HPP
#define FOLDERCONFIG_HPP

#include <string>

class FolderConfig {
private:
    FolderConfig();
    FolderConfig(const FolderConfig& other);
    FolderConfig& operator=(const FolderConfig& other);

    std::string _rootPath;
    bool _enableListing;  // default false
    std::string _indexPageFileLocation;

public:
    FolderConfig(
        const std::string& rootPath,
        bool enableListing,
        const std::string& indexPageFileLocation
    );
    ~FolderConfig();
};

#endif
