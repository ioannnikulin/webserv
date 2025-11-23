#include "UploadConfig.hpp"

namespace webserver {
UploadConfig::UploadConfig(const UploadConfig& other)
    : _uploadEnabled(other._uploadEnabled)
    , _uploadRootFolder(other._uploadRootFolder) {
}

bool UploadConfig::operator==(const UploadConfig& other) const {
    return (_uploadEnabled == other._uploadEnabled && _uploadRootFolder == other._uploadRootFolder);
}

UploadConfig::~UploadConfig() {
}
}  // namespace webserver
