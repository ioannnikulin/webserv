#ifndef UPLOADCONFIG_HPP
#define UPLOADCONFIG_HPP

#include <string>

namespace webserver {
class UploadConfig {
private:
    bool _uploadEnabled;
    std::string _uploadRootFolder;

public:
    UploadConfig();
    UploadConfig(bool uploadEnabled, const std::string& uploadRootFolder);
    UploadConfig(const UploadConfig& other);
    UploadConfig& operator=(const UploadConfig& other);
    ~UploadConfig();

    bool operator==(const UploadConfig& other) const;
    bool operator!=(const UploadConfig& other) const;
    bool isUploadEnabled() const;
    std::string getUploadRootFolder() const;
    friend std::ostream& operator<<(std::ostream& oss, const UploadConfig& config);
};
}  // namespace webserver

#endif
