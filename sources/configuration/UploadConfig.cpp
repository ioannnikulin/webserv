#include "configuration/UploadConfig.hpp"

#include <iostream>
#include <string>

using std::ostream;
using std::string;

namespace webserver {
UploadConfig::UploadConfig()
    : _uploadEnabled(false)
    , _uploadRootFolder("") {
}

UploadConfig::UploadConfig(bool uploadEnabled, const string& uploadRootFolder)
    : _uploadEnabled(uploadEnabled)
    , _uploadRootFolder(uploadRootFolder) {
}

UploadConfig::UploadConfig(const UploadConfig& other)
    : _uploadEnabled(other._uploadEnabled)
    , _uploadRootFolder(other._uploadRootFolder) {
}

bool UploadConfig::operator==(const UploadConfig& other) const {
    return (_uploadEnabled == other._uploadEnabled && _uploadRootFolder == other._uploadRootFolder);
}

bool UploadConfig::operator!=(const UploadConfig& other) const {
    return (!(*this == other));
}

UploadConfig& UploadConfig::operator=(const UploadConfig& other) {
    if (this == &other) {
        return (*this);
    }
    _uploadEnabled = other._uploadEnabled;
    _uploadRootFolder = other._uploadRootFolder;
    return (*this);
}

bool UploadConfig::isUploadEnabled() const {
    return (_uploadEnabled);
}

string UploadConfig::getUploadRootFolder() const {
    return (_uploadRootFolder);
}

UploadConfig::~UploadConfig() {
}

ostream& operator<<(ostream& oss, const UploadConfig& config) {
    oss << config._uploadEnabled;
    oss << " " << config._uploadRootFolder;
    oss << "\n";
    return (oss);
}
}  // namespace webserver
