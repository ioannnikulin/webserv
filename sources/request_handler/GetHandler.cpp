#include "GetHandler.hpp"

#include <dirent.h>

#include <cstddef>
#include <set>
#include <sstream>
#include <string>

#include "configuration/RouteConfig.hpp"
#include "file_system/FileSystem.hpp"
#include "file_system/MimeType.hpp"
#include "http_status/HttpStatus.hpp"
#include "logger/Logger.hpp"
#include "response/Response.hpp"

using std::ostringstream;
using std::set;
using std::string;

namespace webserver {
Logger GetHandler::_log;

string listingItem(string prefix, string name) {
    ostringstream oss;
    oss << "\n<li><a href=\"" << prefix << name << "\">" << name << "</a></li>";
    return (oss.str());
}

Response GetHandler::listDirectory(
    string originalTarget,
    string resolvedTarget,
    const RouteConfig& configuration
) {
    DIR* dir = opendir(resolvedTarget.c_str());
    if (dir == NULL) {
        return (configuration.getStatusCatalogue().serveStatusPage(HttpStatus::INTERNAL_SERVER_ERROR
        ));
    }
    set<string> files;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        const string name(entry->d_name);
        if (name == "." || name == "..") {
            continue;
        }
        files.insert(name);
    }
    closedir(dir);
    ostringstream oss;
    oss << "<html><head><title>" << originalTarget << "</title></head><body>";
    oss << "Contents of folder " << originalTarget << ":\n<hr/>\n<ul>";
    oss << listingItem(originalTarget, ".");
    oss << listingItem(originalTarget, "..");
    for (set<string>::const_iterator itr = files.begin(); itr != files.end(); itr++) {
        oss << listingItem(originalTarget, *itr);
    }
    oss << "\n</ul></body></html>";
    return (Response(
        HttpStatus::OK,
        configuration.getStatusCatalogue().getReasonPhrase(HttpStatus::OK),
        oss.str(),
        MimeType::getMimeType("html")
    ));
}

Response GetHandler::handleRequest(
    string originalTarget,
    string resolvedTarget,
    const RouteConfig& routeConfig
) {
    if (file_system::isDirectory(resolvedTarget.c_str())) {
        _log.stream(LOG_TRACE) << "Target is a directory.\n";
        if (resolvedTarget.at(resolvedTarget.size() - 1) != '/') {
            resolvedTarget = resolvedTarget + "/";
        }
        if (originalTarget.at(originalTarget.size() - 1) != '/') {
            originalTarget = originalTarget + "/";
        }
        string existingIndexFile;
        if (!routeConfig.getFolderConfig().getIndexPageFilename().empty()) {
            existingIndexFile =
                resolvedTarget + routeConfig.getFolderConfig().getIndexPageFilename();
        }
        if (file_system::fileExists(existingIndexFile.c_str())) {
            _log.stream(LOG_TRACE) << "index file available\n";
            resolvedTarget = existingIndexFile;
        } else {  // NOTE: index file doesn't exist, autolisting or 403
            _log.stream(LOG_TRACE) << "index file unavailable, trying to autolist if possible\n";
            _log.stream(LOG_TRACE) << "GET " << resolvedTarget << "\n";
            if (routeConfig.getFolderConfig().isListingEnabled()) {
                return (listDirectory(originalTarget, resolvedTarget, routeConfig));
            }
            return (routeConfig.getStatusCatalogue().serveStatusPage(HttpStatus::FORBIDDEN));
        }
    } else if (file_system::isFile(resolvedTarget.c_str())) {
        _log.stream(LOG_DEBUG) << "Target is a file.\n";
        // NOTE: file exists as is
    } else {
        return (routeConfig.getStatusCatalogue().serveStatusPage(HttpStatus::NOT_FOUND));
    }

    _log.stream(LOG_TRACE) << "GET " << resolvedTarget << "\n";

    if (resolvedTarget.find("..") != std::string::npos) {
        _log.stream(LOG_WARN) << "Directory traversal attempt: " << resolvedTarget << "\n";
        return (routeConfig.getStatusCatalogue().serveStatusPage(HttpStatus::FORBIDDEN));
    }

    if (file_system::fileExists(resolvedTarget.c_str())) {
        return (file_system::serveFile(
            resolvedTarget,
            HttpStatus::OK,
            routeConfig.getStatusCatalogue().getReasonPhrase(HttpStatus::OK)
        ));
    }

    return (routeConfig.getStatusCatalogue().serveStatusPage(HttpStatus::NOT_FOUND));
}

}  // namespace webserver
