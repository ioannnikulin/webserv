#ifndef UPLOADCONFIG_HPP
#define UPLOADCONFIG_HPP

#include <string>

namespace webserver {
class UploadConfig {
private:
    UploadConfig();

    bool _uploadEnabled;
    const std::string _uploadRootFolder;

public:
    UploadConfig(bool uploadEnabled, const std::string& uploadRootFolder);
    UploadConfig(const UploadConfig& other);
    UploadConfig& operator=(const UploadConfig& other);
    ~UploadConfig();

    bool operator==(const UploadConfig& other) const;

    bool isEnabled() const;
    const std::string& getUploadPath() const;
};
}  // namespace webserver

#endif
