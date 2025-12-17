#ifndef FOLDERCONFIG_HPP
#define FOLDERCONFIG_HPP

#include <string>

namespace webserver {
class FolderConfig {
private:
    FolderConfig();

    std::string _rootPath;
    bool _enableListing;  // TODO 15: default false
    std::string _indexPageFileLocation;

public:
    FolderConfig(const FolderConfig& other);
    FolderConfig& operator=(const FolderConfig& other);
    FolderConfig(
        const std::string& rootPath,
        bool enableListing,
        const std::string& indexPageFileLocation
    );

    std::string getRootPath() const;
    std::string getIndexPageFileLocation() const;

    bool operator==(const FolderConfig& other) const;
    ~FolderConfig();
};
}  // namespace webserver

#endif
