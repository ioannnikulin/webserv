#pragma once
#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <map>
#include <vector>

#include "../http_methods/HttpMethodType.hpp"
#include "CgiHandlerConfig.hpp"
#include "FolderConfig.hpp"
#include "UploadConfig.hpp"

namespace webserver {
class RouteConfig {
private:
    RouteConfig(const RouteConfig& other);
    RouteConfig& operator=(const RouteConfig& other);

    std::vector<HttpMethodType> _allowedMethods;
    std::map<std::string, std::string> _redirections;  // NOTE: from:to
    FolderConfig* _folderConfigSection;
    UploadConfig* _uploadConfigSection;
    std::map<std::string, CgiHandlerConfig> _cgiHandlers;  // NOTE: extension:config

public:
    RouteConfig();
    ~RouteConfig();
};
}  // namespace webserver

#endif
