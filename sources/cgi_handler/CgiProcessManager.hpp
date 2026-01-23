#ifndef CGIPROCESSMANAGER_HPP
#define CGIPROCESSMANAGER_HPP

#include <sys/types.h>
#include <time.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include "listener/Listener.hpp"
#include "logger/Logger.hpp"

namespace webserver {
class CgiProcessManager {
public:
    static const int DEFAULT_CGI_TIMEOUT_SECONDS = 30;

    struct CgiPipes {
        int toProcess[2];
        int fromProcess[2];
        int control[2];
    };

    explicit CgiProcessManager(Logger& logger);
    ~CgiProcessManager();

    pid_t startCgiProcess(
        Listener* listener,
        int clientFd,
        const std::string& requestBody,
        int& controlPipeReadEnd,
        int& responsePipeReadEnd
    );

    static std::string parseCgiResponse(const std::string& cgiOutput);
    std::vector<int> checkTimeouts();
    void registerWorker(int clientFd, pid_t pid);
    bool isWorker(int clientFd) const;
    void unregisterWorker(int clientFd);
    void cleanupProcess(int clientFd);
    pid_t getProcessId(int clientFd) const;

private:
    Logger& _log;
    std::set<int> _cgiWorkers;
    std::map<int, pid_t> _cgiProcesses;
    std::map<int, time_t> _cgiStartTimes;
    std::map<int, int> _cgiTimeouts;

    static CgiPipes createPipes();
    static void closePipes(const CgiPipes& pipes);
    static void setNonBlocking(int fileDescriptor);
    static void setupParentPipes(const CgiPipes& pipes);
    static void writeRequestBodyToPipe(int pipeFd, const std::string& requestBody);

    void runCgiChild(
        Listener* listener,
        int clientFd,
        int pipeToProcess[2],
        int pipeFromProcess[2],
        int controlPipe[2]
    );

    static void parseCgiResponseLoop(
        const std::string& headers,
        int& statusCode,
        std::string& contentType,
        std::map<std::string, std::string>& customHeaders
    );

    CgiProcessManager(const CgiProcessManager&);
    CgiProcessManager& operator=(const CgiProcessManager&);
};

}  // namespace webserver

#endif
