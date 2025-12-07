#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cctype>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#include "ConfigParser.hpp"

using std::runtime_error;
using std::string;

namespace webserver {
bool ConfigParser::isEnd(const std::vector<string>& tokens, size_t index) {
    return (index >= tokens.size());
}

namespace {
string readFile(const string& filename) {
    const int fileDescriptor = open(filename.c_str(), O_RDONLY);
    if (fileDescriptor < 0) {
        throw runtime_error("Failed to open config file '" + filename + "'");
    }
    struct stat fileStat;
    if (stat(filename.c_str(), &fileStat) < 0) {
        close(fileDescriptor);
        throw runtime_error("Failed to stat file '" + filename + "'");
    }

    string content;
    content.reserve(fileStat.st_size);

    char buffer[ConfigParser::BUFFER_SIZE];
    ssize_t bytesRead;

    while ((bytesRead = read(fileDescriptor, buffer, sizeof(buffer))) > 0) {
        content.append(buffer, bytesRead);
    }

    const bool readFailed = (bytesRead < 0);

    if (close(fileDescriptor) < 0) {
        throw runtime_error("Error closing file '" + filename + "'");
    }

    if (readFailed) {
        throw runtime_error("Error reading file '" + filename + "'");
    }
    return (content);
}
}  // namespace

void ConfigParser::tokenize(const string& filename) {
    string content = readFile(filename);
    _tokens.clear();
    _index = 0;

    string token;
    for (size_t i = 0; i < content.size(); ++i) {
        const char currentChar = content[i];

        if (isspace(static_cast<unsigned char>(currentChar)) != 0) {
            if (!token.empty()) {
                _tokens.push_back(token);
                token.clear();
            }
            continue;
        }

        if (currentChar == '{' || currentChar == '}' || currentChar == ';') {
            if (!token.empty()) {
                _tokens.push_back(token);
                token.clear();
            }
            _tokens.push_back(string(1, currentChar));
            continue;
        }

        token += currentChar;
    }

    if (!token.empty()) {
        _tokens.push_back(token);
    }
}
}  // namespace webserver
