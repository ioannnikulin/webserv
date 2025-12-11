#ifndef MIMETYPES_HPP
#define MIMETYPES_HPP

#include <map>
#include <string>

namespace webserver {
class MimeTypes {
private:
    static std::map<std::string, std::string> mimeMap;
    static const std::string DEFAULT_MIME;

    MimeTypes();
    MimeTypes(const MimeTypes& other);
    MimeTypes& operator=(const MimeTypes& other);

    static std::map<std::string, std::string> initMimeMap();

public:
    ~MimeTypes();

    static std::string getMimeType(const std::string& extension);
};
}  // namespace webserver

#endif
