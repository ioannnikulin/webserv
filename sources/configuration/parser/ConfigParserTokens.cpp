
#include <cctype>
#include <cerrno>
#include <cstring>
#include <string>
#include <vector>

#include "configuration/parser/ConfigParser.hpp"
#include "file_system/FileSystem.hpp"

using std::string;

namespace webserver {
bool ConfigParser::isEnd(const std::vector<string>& tokens, size_t index) {
    return (index >= tokens.size());
}

void ConfigParser::tokenize(const string& filename) {
    string content = file_system::readFile(filename.c_str());
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
