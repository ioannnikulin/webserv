#include <cctype>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "configuration/UploadConfig.hpp"
#include "configuration/parser/ConfigParser.hpp"
#include "http_status/HttpStatus.hpp"
#include "utils/utils.hpp"

using std::istringstream;
using std::runtime_error;
using std::string;

namespace webserver {
void ConfigParser::parseListen(Endpoint& server) {
    _index++;
    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected value after 'listen'");
    }

    const string value = _tokens[_index];
    _index++;

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after listen directive");
    }
    _index++;

    const size_t colonPos = value.find(":");

    if (colonPos == string::npos) {
        int port;
        istringstream iss(value);
        iss >> port;
        if (!Endpoint::isAValidPort(port)) {
            throw runtime_error("Invalid port in listen: " + value);
        }
        server.setInterface("0.0.0.0");
        server.setPort(port);
        return;
    }

    const string interface = value.substr(0, colonPos);
    const string portStr = value.substr(colonPos + 1);

    int port;
    istringstream iss(portStr);
    iss >> port;
    if (!Endpoint::isAValidPort(port)) {
        throw runtime_error("Invalid port in listen: " + value);
    }
    if (interface.empty()) {
        throw runtime_error("Invalid host in listen: " + value);
    }
    server.setInterface(interface);
    server.setPort(port);
}

void ConfigParser::parseServerName(Endpoint& server) {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected a name after 'server_name'");
    }

    std::string name;

    name = _tokens[_index];

    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Missing ';' after server_name directive");
    }

    _index++;

    if (name.empty()) {
        throw runtime_error("No server_name value provided");
    }
    server.addServerName(name);
}

void ConfigParser::parseRoot(Endpoint& server) {
    _index++;
    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected path after 'root'");
    }

    const string path = _tokens[_index];
    _index++;

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after root directive");
    }

    _index++;

    server.setRoot(path);
}

namespace {
size_t parseSizeValue(const string& value) {
    int multiplier = 1;
    string numbers = value;

    const char last = value[value.size() - 1];
    if (last == 'K' || last == 'k') {
        multiplier = utils::KIB;
        numbers = value.substr(0, value.size() - 1);
    } else if (last == 'M' || last == 'm') {
        multiplier = utils::MIB;
        numbers = value.substr(0, value.size() - 1);
    } else if (last == 'G' || last == 'g') {
        multiplier = utils::GIB;
        numbers = value.substr(0, value.size() - 1);
    }

    long num;
    istringstream iss(numbers);
    iss >> num;
    if (num < 0) {
        throw runtime_error("Invalid size: " + value);
    }

    return (static_cast<size_t>(num) * multiplier);
}
}  // namespace

void ConfigParser::parseBodySize(Endpoint& server) {
    _index++;
    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected value after 'client_max_body_size'");
    }

    const string value = _tokens[_index];
    _index++;

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after client_max_body_size");
    }

    _index++;

    const size_t size = parseSizeValue(value);
    server.setClientMaxBodySizeBytes(size);
}

void ConfigParser::parseErrorPage() {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected status code or URL after 'error_page'");
    }

    std::vector<int> codes;
    while (!isEnd(_tokens, _index)) {
        if (_tokens[_index] == ";") {
            throw runtime_error("Missing path in error_page directive");
        }
        if (_tokens[_index][0] == '/' || _tokens[_index].find('.') != string::npos) {
            break;
        }
        int code;
        istringstream iss(_tokens[_index]);
        iss >> code;
        if (!HttpStatus::isAValidHttpStatusCode(code)) {
            throw runtime_error("Invalid HTTP status code in error_page: " + _tokens[_index]);
        }
        codes.push_back(code);
        _index++;
    }

    if (codes.empty()) {
        throw runtime_error("No status codes provided in error_page directive");
    }

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected path after status codes in error_page");
    }

    const string pagePath = _tokens[_index++];

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after error_page directive");
    }

    _index++;

    for (size_t i = 0; i < codes.size(); i++) {
        HttpStatus::setPage(codes[i], pagePath);
    }
}

void ConfigParser::parseCgi(Endpoint& server) {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected CGI extension after 'cgi'");
    }

    string extension = _tokens[_index];

    _index++;

    if (extension[0] != '.') {
        throw runtime_error("Invalid CGI extension: " + extension);
    }

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected CGI executable path after extension");
    }

    const string execPath = _tokens[_index];

    _index++;

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after cgi directive");
    }

    _index++;

    const CgiHandlerConfig config(30, execPath);

    server.addCgiHandler(config, extension);
}

void ConfigParser::parseUpload(Endpoint& server) {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected 'on' or 'off' after 'upload'");
    }

    const string enabled = _tokens[_index];

    _index++;

    bool uploadEnabled;
    if (enabled == "on") {
        uploadEnabled = true;
    } else if (enabled == "off") {
        uploadEnabled = false;
    } else {
        throw runtime_error("upload must be 'on' or 'off' at server level");
    }

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected upload directory after upload");
    }

    const string uploadRoot = _tokens[_index];

    _index++;

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after upload directive");
    }

    _index++;

    const UploadConfig cfg(uploadEnabled, uploadRoot);
    server.setUploadConfig(cfg);
}
}  // namespace webserver
