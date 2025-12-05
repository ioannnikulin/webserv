#include "FileSystem.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdexcept>

#define DEF_BUFFER_SIZE 4096

namespace file_system {

bool fileExists(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0)
        return true;
    return false;
}

int validateFile(const char* path) {
    if (!fileExists(path))
        return 404;
    return 200;
}

long getFileSize(const char* path) {
    struct stat st;
    if (stat(path, &st) == 0)
        return (st.st_size);
    return (-1);
}

std::string readFile(const char* path) {
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        throw std::runtime_error("Failed to open file");

    char buffer[DEF_BUFFER_SIZE];
    std::string result;

    ssize_t bytes;
    while ((bytes = read(fd, buffer, sizeof(buffer))) > 0) {
        result.append(buffer, bytes);
    }
    if (bytes < 0)
        throw std::runtime_error("Failed to read file");
    close(fd);

    return (result);
}

std::string getFileExtension(const std::string& path) {
    std::string::size_type slash = path.find_last_of("/\\");
    std::string::size_type dot = path.find_last_of('.');

    if (dot == std::string::npos || (slash != std::string::npos && dot < slash)) {
        return "";
    }

    return path.substr(dot + 1);
}
}  // namespace file_system
