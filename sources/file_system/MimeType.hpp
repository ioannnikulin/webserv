#ifndef MIMETYPE_HPP
#define MIMETYPE_HPP

#include <map>
#include <string>

namespace webserver {
class MimeType {
private:
    std::string _type;
    bool _isPrintable;
    static std::map<std::string, MimeType> _mimeTypeMap;
    static const std::string DEFAULT_MIME_TYPE;

    MimeType(const std::string& type, bool isPrintable);
    MimeType& operator=(const MimeType& other);

    static std::map<std::string, MimeType> initMimeTypeMap();

public:
    MimeType();
    MimeType(const MimeType& other);
    ~MimeType();

    static std::string getMimeType(const std::string& extension);
    static bool isPrintable(const std::string& mimeType);
};
}  // namespace webserver

#endif
