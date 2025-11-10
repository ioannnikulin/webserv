namespace webserver {
class RequestHandler {
private:
    RequestHandler();
    RequestHandler(const RequestHandler& other);
    RequestHandler& operator=(const RequestHandler& other);

public:
    static std::string handle(std::string requestBody);
    ~RequestHandler();
};
}  // namespace webserver