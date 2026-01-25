#ifndef HTTPSTATUS_HPP
#define HTTPSTATUS_HPP

#include <map>
#include <string>

#include "response/Response.hpp"

namespace webserver {
class HttpStatus {
private:
    class Item {
    private:
        int _code;
        std::string _reasonPhrase;
        // NOTE: custom page that can be set via config. actually used; taken from config if present, else set to default
        std::string _pageFileLocation;
        static std::string defaultFolder();

    public:
        Item();
        Item(int code, const std::string& reasonPhrase);
        Item(int code, const std::string& reasonPhrase, const std::string& page);
        Item(const Item& other);
        ~Item();
        const std::string& getReasonPhrase() const;
        bool operator<(const Item& other) const;
        Item& operator=(const Item& other);
        bool operator==(const Item& other) const;
        std::string getDefaultPageFileLocation() const;
        const std::string& getPageFileLocation() const;
        Item& setPageFileLocation(std::string location);
        void print(std::ostream& oss) const;
    };
    static const int MIN_CODE;
    static const int MAX_CODE;

    // NOTE: subject suggests we have same pages for all endpoints, so static for now
    std::map<int, Item> _statusMap;
    static const std::map<int, HttpStatus::Item>& defaultStatusMap();

    static void addStatus(std::map<int, Item>& map, int code, const std::string& reasonPhrase);
    static std::map<int, Item> createDefaultStatusMap();
    static Response serveStatusPage(int code, std::string reasonPhrase, std::string uncheckedPath);

public:
    HttpStatus();
    HttpStatus(const HttpStatus& other);
    ~HttpStatus();
    HttpStatus& operator=(const HttpStatus& other);
    bool operator==(const HttpStatus& other) const;
    bool operator!=(const HttpStatus& other) const;

    static const std::string UNKNOWN_STATUS;

    // NOTE: these three methods should throw different exceptions if status code not found or status code map is empty
    std::string getReasonPhrase(int code) const;
    static bool isAValidHttpStatusCode(int code);

    HttpStatus& setPage(int code, const std::string& pageFileLocation);

    const std::string& getPageFileLocation(int code) const;
    Response serveStatusPage(int statusCode) const;
    static Response ultimateInternalServerError();
    // NOTE: uncustomized, as default as possible, static. use in emergency.

    enum CODE {
        OK = 200,
        CREATED = 201,
        ACCEPTED = 202,
        NO_CONTENT = 204,
        MOVED_PERMANENTLY = 301,
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
        HTTP_SERVICE_UNAVAILABLE = 503,
        GATEWAY_TIMEOUT = 504,
        HTTP_VERSION_NOT_SUPPORTED = 505
    };
    friend std::ostream& operator<<(std::ostream& oss, const HttpStatus& catalogue);
};
}  // namespace webserver

#endif
