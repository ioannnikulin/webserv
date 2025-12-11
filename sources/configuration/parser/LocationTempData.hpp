#ifndef LOCATION_TEMP_DATA_HPP
#define LOCATION_TEMP_DATA_HPP

#include <string>

namespace webserver {
class LocationTempData {
private:
    LocationTempData(const LocationTempData& other);
    LocationTempData& operator=(const LocationTempData& other);

    std::string _rootPath;
    bool _rootSet;

    bool _autoindex;
    bool _autoindexSet;

    std::string _indexPage;
    bool _indexSet;

    bool _uploadEnabled;
    std::string _uploadRoot;
    bool _uploadSet;

public:
    LocationTempData();
    ~LocationTempData();

    void clear();

    void setRootPath(const std::string& path);
    const std::string& rootPath() const;
    bool rootSet() const;

    void setAutoindex(bool value);
    bool autoindex() const;
    bool autoindexSet() const;

    void setIndexPage(const std::string& page);
    const std::string& indexPage() const;
    bool indexSet() const;

    void setUploadEnabled(bool value);
    bool uploadEnabled() const;

    void setUploadRoot(const std::string& path);
    const std::string& uploadRoot() const;
    bool uploadSet() const;
};
}  // namespace webserver

#endif
