#pragma once
#ifndef ROUTE_HPP
#define ROUTE_HPP

#include <map>
#include <set>
#include <vector>

#include "configuration/CgiHandlerConfig.hpp"
#include "configuration/FolderConfig.hpp"
#include "configuration/UploadConfig.hpp"
#include "http_methods/HttpMethodType.hpp"

namespace webserver {
class RouteConfig {
private:
    std::string _path;
    std::set<HttpMethodType> _allowedMethods;
    std::map<std::string, std::string> _redirections;  // NOTE: from:to
    FolderConfig* _folderConfigSection;
    UploadConfig* _uploadConfigSection;
    std::map<std::string, CgiHandlerConfig> _cgiHandlers;  // NOTE: extension:config

public:
    RouteConfig();
    RouteConfig& operator=(const RouteConfig& other);
    RouteConfig(const RouteConfig& other);

    RouteConfig& setFolderConfig(const FolderConfig& tgt);
    const FolderConfig* getFolderConfig() const;

    bool operator==(const RouteConfig& other) const;
    void addAllowedMethod(HttpMethodType method);
    void addRedirection(const std::string& from, const std::string& toDir);
    RouteConfig& setFolderConfig(const FolderConfig& folder);
    void setUploadConfig(const UploadConfig& upload);
    void setPath(std::string path);
    void addCgiHandler(const CgiHandlerConfig& cfg, std::string extension);
    std::string getPath() const;
};
}  // namespace webserver

#endif
