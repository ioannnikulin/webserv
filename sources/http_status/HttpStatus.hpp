#ifndef HTTPSTATUS_HPP
#define HTTPSTATUS_HPP

#include <map>
#include <string>

namespace webserver {

class HttpStatus {
private:
    static const int MIN_CODE;
    static const int MAX_CODE;
    static const std::string DEFAULT_STATUS_PAGE_DIR;

    HttpStatus();

    // NOTE: subject suggests we have same pages for all endpoints, so static for now
    static std::map<int, HttpStatus> _statusMap;

    int _code;
    std::string _reasonPhrase;
    // NOTE: default page. hardcoded in initStatusMap(); no accessors
    std::string _defaultPageFileLocation;
    // NOTE: custom page that can be set via config. actually used; taken from config if present, else set to default
    std::string _pageFileLocation;
    static void addStatus(int code, const std::string& reasonPhrase);

public:
    HttpStatus(int code, const std::string& reasonPhrase);
    HttpStatus(int code, const std::string& reasonPhrase, const std::string& page);
    HttpStatus(const HttpStatus& other);
    HttpStatus& operator=(const HttpStatus& other);
    ~HttpStatus();

    static void initStatusMap();

    static const std::string UNKNOWN_STATUS;

    // NOTE: these three methods should throw different exceptions if status code not found or status code map is empty
    static std::string getReasonPhrase(int code);
    static std::string getDefaultPageLocation(int code);
    static bool isAValidHttpStatusCode(int code);

    static void setPage(int code, const std::string& pageFileLocation);

    static std::string getPageFileLocation(int code);

    enum CODE {
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NO_CONTENT = 204,
        BAD_REQUEST = 400,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        METHOD_NOT_ALLOWED = 405,
        PAYLOAD_TOO_LARGE = 413,
        I_AM_A_TEAPOT = 418,
        REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
        INTERNAL_SERVER_ERROR = 500,
        NOT_IMPLEMENTED = 501,
        BAD_GATEWAY = 502,
        HTTP_VERSION_NOT_SUPPORTED = 505
    };
};
}  // namespace webserver

#endif
