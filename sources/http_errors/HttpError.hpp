#ifndef HTTPERROR_HPP
#define HTTPERROR_HPP

#include <map>
#include <string>

class HttpError {
private:
    HttpError();
    HttpError(const HttpError& other);
    HttpError& operator=(const HttpError& other);
    HttpError(int code, const std::string& message);
    HttpError(int code, const std::string& message, const std::string& page);

    const std::string _message;
    const std::string _defaultPageFileLocation;  // hardcoded in initializeErrors; no accessors
    const std::string _pageFileLocation;  // actually used; take from config, else pickup default

    // subject suggests we have same pages for all endpoints, so static for now
    static std::map<int, HttpError> _errors;

public:
    void initializeErrors();  // fills map; called once at server start
    ~HttpError();

    // next three throw different exceptions if code not found or map empty
    std::string getMessage(int code) const;
    void setPage(int code, const std::string& pageFileLocation);  // used in configuration parser
    std::string getPageContent(int code) const;                   // read file, return its content

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

#endif