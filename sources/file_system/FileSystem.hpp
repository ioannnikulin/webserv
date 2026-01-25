#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>

#include "http_status/HttpStatus.hpp"
#include "response/Response.hpp"

namespace file_system {
bool isFile(const char* path);
bool isDirectory(const char* path);
bool fileExists(const char* path);
webserver::HttpStatus::CODE validateFile(const char* path);
long getFileSize(const char* path);
std::string readFile(const char* path);
std::string getFileExtension(const std::string& path);
bool isReadableFile(const char* path);
bool isExecutableFile(const char* path);
bool isWritableDirectory(const char* path);
bool canCreateDirectory(const char* path);
webserver::Response serveFile(const std::string& path, int statusCode, std::string reasonPhrase);
}  // namespace file_system

#endif
