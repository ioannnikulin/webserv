#pragma once
#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <map>
#include <vector>

#include "../http_methods/HttpMethodType.hpp"
#include "CgiHandlerConfig.hpp"
#include "FolderConfig.hpp"
#include "UploadConfig.hpp"

class RouteConfig {
private:
    RouteConfig(const RouteConfig& other);
    RouteConfig& operator=(const RouteConfig& other);

    std::vector<HttpMethodType> _allowedMethods;
    std::map<std::string, std::string> _redirections;  // from:to
    FolderConfig* _folderConfigSection;
    UploadConfig* _uploadConfigSection;
    std::map<std::string, CgiHandlerConfig> _cgiHandlers;  // extension:config

public:
    RouteConfig();
    ~RouteConfig();
};

#endif
