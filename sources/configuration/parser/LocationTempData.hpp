#ifndef LOCATIONTEMPDATA_HPP
#define LOCATIONTEMPDATA_HPP

#include <string>

namespace webserver {
class LocationTempData {
private:
    LocationTempData(const LocationTempData& other);
    LocationTempData& operator=(const LocationTempData& other);

    std::string _storageRootPath;
    bool _rootSet;

    bool _listable;
    bool _listableSet;

    std::string _indexPage;
    bool _indexSet;

    bool _uploadEnabled;
    std::string _uploadRoot;
    bool _uploadSet;

    bool _maxBodySizeBytesSet;
    size_t _maxClientBodySizeBytes;

public:
    LocationTempData();
    ~LocationTempData();

    void clear();

    void setRootPath(const std::string& path);
    const std::string& rootPath() const;
    bool rootSet() const;

    void setListable(bool value);
    bool listable() const;
    bool listableSet() const;

    void setIndexPage(const std::string& page);
    const std::string& indexPage() const;
    bool indexSet() const;

    void setUploadEnabled(bool value);
    bool uploadEnabled() const;

    void setUploadRoot(const std::string& path);
    const std::string& uploadRoot() const;
    bool uploadSet() const;

    void setMaxBodySizeBytes(size_t maxBodySizeBytes);
    size_t getMaxBodySizeBytes() const;
    bool maxBodySizeBytesSet() const;
};
}  // namespace webserver

#endif
