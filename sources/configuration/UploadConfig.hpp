#ifndef UPLOADCONFIG_HPP
#define UPLOADCONFIG_HPP

#include <string>

namespace webserver {
class UploadConfig {
private:
    UploadConfig();

    bool _uploadEnabled;  // TODO 15: default false
    const std::string _uploadRootFolder;

public:
    UploadConfig(bool uploadEnabled, const std::string& uploadRootFolder);
    UploadConfig(const UploadConfig& other);
    UploadConfig& operator=(const UploadConfig& other);
    ~UploadConfig();

    bool operator==(const UploadConfig& other) const;
};
}  // namespace webserver

#endif
