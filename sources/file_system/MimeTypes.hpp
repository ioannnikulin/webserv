#ifndef MIMETYPES_HPP
#define MIMETYPES_HPP

#include <map>
#include <string>

class MimeTypes {
private:
    static const std::map<std::string, std::string> mimeMap;

    MimeTypes();
    MimeTypes(const MimeTypes& other);
    MimeTypes& operator=(const MimeTypes& other);

    static std::map<std::string, std::string> initMimeMap();

public:
    ~MimeTypes();

    static std::string getMimeType(const std::string& extenstion);
};

#endif
