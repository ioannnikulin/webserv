#ifndef FOLDERCONFIG_HPP
#define FOLDERCONFIG_HPP

#include <string>

namespace webserver {
class FolderConfig {
private:
    // NOTE: e.g. /
    // NOTE: yes, this field duplicates key in Endpoint's map
    std::string _requestedLocation;

    // NOTE: e.g. /var/www/html
    std::string _storageRootPath;

    bool _enableListing;

    // NOTE: only a name of the file that we have to look for in every folder in this route
    // NOTE: individual file per folder, not a common indexer for whole route
    std::string _indexPageFilename;
    size_t _maxClientBodySizeBytes;

public:
    static size_t defaultMaxClientBodySizeBytes();
    FolderConfig();
    FolderConfig(const FolderConfig& other);
    FolderConfig& operator=(const FolderConfig& other);
    FolderConfig(
        const std::string& location,
        const std::string& rootPath,
        bool enableListing,
        const std::string& indexPageFilename,
        size_t maxClientBodySizeBytes
    );

    std::string getRootPath() const;
    std::string getResolvedPath(std::string target) const;
    std::string getIndexPageFilename() const;
    bool isListingEnabled() const;
    size_t getMaxClientBodySizeBytes() const;
    bool operator==(const FolderConfig& other) const;
    bool operator!=(const FolderConfig& other) const;
    ~FolderConfig();
    friend std::ostream& operator<<(std::ostream& oss, const FolderConfig& config);
};
}  // namespace webserver

#endif
