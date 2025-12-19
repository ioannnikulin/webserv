#include "FileSystem.hpp"

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdexcept>
#include <string>

#include "http_status/HttpStatus.hpp"

#define DEFAULT_BUFFER_SIZE 4096

namespace file_system {

bool fileExists(const char* path) {
    struct stat fileStat;
    if (stat(path, &fileStat) == 0) {
        return (true);
    }
    return (false);
}

int validateFile(const char* path) {
    if (!fileExists(path)) {
        return (webserver::HttpStatus::NOT_FOUND);
    }
    return (webserver::HttpStatus::OK);
}

long getFileSize(const char* path) {
    struct stat fileStat;
    if (stat(path, &fileStat) == 0) {
        return (fileStat.st_size);
    }
    return (-1);
}

std::string readFile(const char* path) {
    const int fileDescriptor = open(path, O_RDONLY);
    if (fileDescriptor < 0) {
        throw std::runtime_error("Failed to open file");
    }

    char buffer[DEFAULT_BUFFER_SIZE];
    std::string result;

    ssize_t bytes;
    while ((bytes = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
        result.append(buffer, bytes);
    }
    if (bytes < 0) {
        throw std::runtime_error("Failed to read file");
    }
    close(fileDescriptor);

    return (result);
}

std::string getFileExtension(const std::string& path) {
    /* NOTE: 
    If we find a dot (.) that is located before the last slash, it is part of a directory name, not the file extension
    Example: dir/.config/file
    Without the slash check we will end up with ".config/file" being our file extension, which is wrong.
    With the slash check, we get "" as the extension, which is correct.
    */
    const std::string::size_type slash = path.find_last_of("/\\");
    const std::string::size_type dot = path.find_last_of('.');

    if (dot == std::string::npos || (slash != std::string::npos && dot < slash)) {
        return "";
    }

    return (path.substr(dot + 1));
}
}  // namespace file_system
