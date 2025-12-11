#include <cctype>
#include <cerrno>
#include <cstring>
#include <stdexcept>
#include <string>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/Endpoint.hpp"
#include "configuration/FolderConfig.hpp"
#include "configuration/RouteConfig.hpp"
#include "configuration/UploadConfig.hpp"
#include "configuration/parser/ConfigParser.hpp"
#include "http_methods/HttpMethodType.hpp"

using std::runtime_error;
using std::string;

namespace webserver {
void ConfigParser::parseLocation(Endpoint& server) {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected path after 'location'");
    }

    const string locationPath = _tokens[_index];
    _tmp.clear();
    _index++;

    if (isEnd(_tokens, _index) || _tokens[_index] != "{") {
        throw runtime_error("Expected '{' after location path '" + locationPath + "'");
    }
    _index++;

    RouteConfig route;

    while (!isEnd(_tokens, _index)) {
        const string& token = _tokens[_index];

        if (token == "root") {
            parseLocationRoot();
        } else if (token == "autoindex") {
            parseLocationAutoindex();
        } else if (token == "index") {
            parseLocationIndex();
        } else if (token == "methods") {
            parseLocationMethods(route);
        } else if (token == "limit_except") {
            parseLocationLimitExcept(route);
        } else if (token == "return") {
            parseLocationReturn(route, locationPath);
        } else if (token == "upload") {
            parseLocationUpload();
        } else if (token == "cgi") {
            parseLocationCgi(route);
        } else if (token != "}") {
            throw runtime_error("Unexpected token in location block: " + token);
        } else {
            _index++;
            break;
        }
    }

    if (isEnd(_tokens, _index) && (_tokens[_index - 1] != "}")) {
        throw runtime_error("Unexpected end of file in location block (missing '}')");
    }

    const FolderConfig folder(
        _tmp.rootSet() ? _tmp.rootPath() : "",
        _tmp.autoindexSet() ? _tmp.autoindex() : false,
        _tmp.indexSet() ? _tmp.indexPage() : ""
    );
    route.setFolderConfig(folder);

    route.setPath(locationPath);

    if (_tmp.uploadSet()) {
        const UploadConfig upload(_tmp.uploadEnabled(), _tmp.uploadRoot());
        route.setUploadConfig(upload);
    }

    server.addRoute(route);
}

void ConfigParser::parseLocationRoot() {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected path after 'root' in location");
    }

    _tmp.setRootPath(_tokens[_index]);
    _index++;

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after root");
    }

    _index++;
}

void ConfigParser::parseLocationAutoindex() {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected 'on' or 'off' after autoindex");
    }

    const std::string val = _tokens[_index++];

    if (val == "on") {
        _tmp.setAutoindex(true);
    } else if (val == "off") {
        _tmp.setAutoindex(false);
    } else {
        throw runtime_error("autoindex must be on/off");
    }

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after autoindex");
    }

    _index++;
}

void ConfigParser::parseLocationIndex() {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected filename after index");
    }

    _tmp.setIndexPage(_tokens[_index]);

    _index++;

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after index");
    }

    _index++;
}

void ConfigParser::parseLocationMethods(RouteConfig& route) {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected methods after 'methods'");
    }

    while (!isEnd(_tokens, _index) && _tokens[_index] != ";") {
        const HttpMethodType method = webserver::stringToMethod(_tokens[_index]);
        route.addAllowedMethod(method);
        _index++;
    }

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after methods in location");
    }

    _index++;
}

void ConfigParser::parseLocationLimitExcept(RouteConfig& route) {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected methods after 'limit_except'");
    }

    while (!isEnd(_tokens, _index) && _tokens[_index] != ";") {
        const HttpMethodType method = webserver::stringToMethod(_tokens[_index]);
        route.addAllowedMethod(method);
        _index++;
    }

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after limit_except in location");
    }

    _index++;
}

void ConfigParser::parseLocationReturn(RouteConfig& route, const string& locationPath) {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected target after 'return' in location");
    }

    const string target = _tokens[_index];

    _index++;

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after return in location");
    }

    _index++;

    route.addRedirection(locationPath, target);
}

void ConfigParser::parseLocationUpload() {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected on/off after upload");
    }

    const string enabled = _tokens[_index];

    _index++;

    if (enabled == "on") {
        _tmp.setUploadEnabled(true);
    } else if (enabled == "off") {
        _tmp.setUploadEnabled(false);
    } else {
        throw runtime_error("upload must be on/off");
    }

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected upload path");
    }

    _tmp.setUploadRoot(_tokens[_index]);

    _index++;

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after upload");
    }

    _index++;
}

void ConfigParser::parseLocationCgi(RouteConfig& route) {
    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected extension after 'cgi'");
    }

    string ext = _tokens[_index];

    if (ext.empty() || ext[0] != '.') {
        throw runtime_error("Invalid CGI extension: " + ext);
    }

    _index++;

    if (isEnd(_tokens, _index)) {
        throw runtime_error("Expected executable path after CGI extension");
    }

    const string execPath = _tokens[_index];

    _index++;

    if (isEnd(_tokens, _index) || _tokens[_index] != ";") {
        throw runtime_error("Missing ';' after cgi directive in location");
    }

    _index++;

    const CgiHandlerConfig cfg(30, execPath);
    route.addCgiHandler(cfg, ext);
}
}  // namespace webserver
