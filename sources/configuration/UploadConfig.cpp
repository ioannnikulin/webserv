#include "configuration/UploadConfig.hpp"

#include <string>

using std::string;
namespace webserver {
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

UploadConfig::~UploadConfig() {
}

bool UploadConfig::isEnabled() const {
    return (_uploadEnabled);
}

const string& UploadConfig::getUploadPath() const {
    return (_uploadRootFolder);
}
}  // namespace webserver
