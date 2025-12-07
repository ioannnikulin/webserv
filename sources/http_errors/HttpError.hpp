#ifndef HTTPERROR_HPP
#define HTTPERROR_HPP

#include <map>
#include <string>

namespace webserver {
class HttpError {
private:
    int _code;
    HttpError(int code, const std::string& message);
    HttpError(int code, const std::string& message, const std::string& page);

    const std::string _message;
    const std::string
        _defaultPageFileLocation;   // NOTE: hardcoded in initializeErrors; no accessors
    std::string _pageFileLocation;  // NOTE: actually used; take from config, else pickup default

    // NOTE: subject suggests we have same pages for all endpoints, so static for now
    static std::map<int, HttpError> _errors;
    static const int MIN_CODE;
    static const int MAX_CODE;

public:
    static void initializeErrors();  // NOTE: fills map; called once at server start
    HttpError();
    HttpError(const HttpError& other);
    HttpError& operator=(const HttpError& other);
    ~HttpError();

    // NOTE: next three throw different exceptions if code not found or map empty
    std::string getMessage(int code) const;
    static void setPage(
        int code,
        const std::string& pageFileLocation
    );                                           // TODO 15: used in configuration parser
    std::string getPageContent(int code) const;  // TODO 18: read file, return its content

    static bool isAValidHttpStatusCode(int code);

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
