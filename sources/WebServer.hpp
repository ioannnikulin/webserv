#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <string>

#include "configuration/AppConfig.hpp"
#include "listener/MasterListener.hpp"

namespace webserver {
class WebServer {
private:
    WebServer();
    // NOTE: DL: The keyword explicit is used to prevent implicit (automatic) type conversions when calling a constructor.
    explicit WebServer(const std::string& configFilePath);
    WebServer(const WebServer& other);
    WebServer& operator=(const WebServer& other);

    AppConfig* _appConfig;
    // NOTE: DL: __sig_atomic_t is a special int type guaranteed to be writable in one atomic CPU operation & safe to modify inside a signal handler
    // NOTE: DL: volatile tells the compiler: “Don’t optimize access to this variable. Always read/write it directly from memory.”
    volatile __sig_atomic_t _isRunning;
    MasterListener* _masterListener;

    static void handleSignals();

public:
    /* NOTE: we're not allowed to use sigaction function, only signal.
	* signal requires a static handler,
	* so we have to use a static variable,
	* so it's either one flag for all instances or a singleton.
	* we stick with singleton.
	*/
    static WebServer& getInstance(const std::string& configFilePath);
    ~WebServer();

    AppConfig getAppConfig() const;

    void start();
    void stop();
};

std::string generateResponse(const std::string& request);
}  // namespace webserver

#endif
