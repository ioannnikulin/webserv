#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>

#include "http_status/HttpStatus.hpp"

namespace file_system {
bool isFile(const char* path);
bool isDirectory(const char* path);
bool fileExists(const char* path);
webserver::HttpStatus::CODE validateFile(const char* path);
long getFileSize(const char* path);
std::string readFile(const char* path);
std::string getFileExtension(const std::string& path);
}  // namespace file_system

#endif
