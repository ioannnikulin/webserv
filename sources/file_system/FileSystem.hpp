#include <string>

namespace file_system {
bool fileExists(const char* path);
int validateFile(const char* path);
long getFileSize(const char* path);
std::string readFile(const char* path);
std::string getFileExtension(const std::string& path);
}  // namespace file_system
