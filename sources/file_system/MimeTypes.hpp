#ifndef MIMETYPES_HPP
#define MIMETYPES_HPP

#include <map>
#include <string>

namespace webserver {
class MimeTypes {
private:
    static std::map<std::string, std::string> _mimeTypeMap;
    static const std::string DEFAULT_MIME_TYPE;

    MimeTypes();
    MimeTypes(const MimeTypes& other);
    MimeTypes& operator=(const MimeTypes& other);

    static std::map<std::string, std::string> initMimeTypeMap();

public:
    ~MimeTypes();

    static std::string getMimeType(const std::string& extension);
};
}  // namespace webserver

#endif
