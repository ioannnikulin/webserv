#ifndef HTTPSTATUS_HPP
#define HTTPSTATUS_HPP

#include <map>
#include <string>

#include "http_methods/HttpMethodType.hpp"

namespace webserver {

class HttpStatus {
private:
    // NOTE: subject suggests we have same pages for all endpoints, so static for now
    HttpStatus();

    static std::map<int, HttpStatus> _statusMap;

    int _code;
    std::string _reasonPhrase;  // NOTE: the reason phrase for the error
    std::string
        _defaultPageFileLocation;  // NOTE: default page for a specific error. hardcoded in initializeErrors; no accessors
    std::string
        _pageFileLocation;  // NOTE: custom page that can be set via config. actually used; take from config, else pickup default

public:
    HttpStatus(int code, const std::string& reasonPhrase);
    HttpStatus(int code, const std::string& reasonPhrase, const std::string& page);
    HttpStatus(const HttpStatus& other);
    HttpStatus& operator=(const HttpStatus& other);
    ~HttpStatus();

    static void initStatusMap();
    /* NOTE:
    fills map; called once at server start
    static void initStatusHelper(
    int code,
    const std::string& reasonPhrase
    */
    // NOTE: helper function for initStatusMap()
    static void addStatus(int code, const std::string& reasonPhrase);
    static const std::string UNKNOWN_STATUS;

    static std::string getReasonPhrase(int code);
    static std::string getDefaultPageLocation(int code);

    // NOTE: next three throw different exceptions if code not found or map empty
    static void setPage(
        int code,
        const std::string& pageFileLocation
    );  // TODO 15: used in configuration parser
    static std::string getPageFileLocation(int code);

    // NOTE: removed / fully replace by serveFile. static ResponseData getPageContent(int code);

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
