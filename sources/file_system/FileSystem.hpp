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
webserver::Response serveFile(const std::string& path, int statusCode);
webserver::Response serveStatusPage(int statusCode);
}  // namespace file_system

#endif
