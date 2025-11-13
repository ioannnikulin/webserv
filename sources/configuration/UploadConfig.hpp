#ifndef UPLOADCONFIG_HPP
#define UPLOADCONFIG_HPP

#include <string>

class UploadConfig {
private:
    UploadConfig();
    UploadConfig(const UploadConfig& other);
    UploadConfig& operator=(const UploadConfig& other);

    bool _uploadEnabled;  // default false
    const std::string _uploadRootFolder;

public:
    UploadConfig(bool uploadEnabled, const std::string& uploadRootFolder);
    ~UploadConfig();
};

#endif
