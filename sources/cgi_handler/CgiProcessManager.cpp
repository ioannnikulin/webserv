#include "CgiProcessManager.hpp"

#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>

#include "connection/Connection.hpp"
#include "http_status/HttpStatus.hpp"
#include "listener/Listener.hpp"
#include "logger/Logger.hpp"
#include "response/Response.hpp"

using std::map;
using std::ostringstream;
using std::runtime_error;
using std::string;
using std::vector;

namespace webserver {

CgiProcessManager::CgiProcessManager(Logger& logger)
    : _log(logger) {
}

CgiProcessManager::~CgiProcessManager() {
}

CgiProcessManager::CgiPipes CgiProcessManager::createPipes() {
    CgiPipes pipes;
    if (pipe(pipes.toProcess) == -1 || pipe(pipes.fromProcess) == -1 || pipe(pipes.control) == -1) {
        throw runtime_error("pipe() failed for CGI");
    }
    return (pipes);
}

void CgiProcessManager::closePipes(const CgiPipes& pipes) {
    close(pipes.toProcess[0]);
    close(pipes.toProcess[1]);
    close(pipes.fromProcess[0]);
    close(pipes.fromProcess[1]);
    close(pipes.control[0]);
    close(pipes.control[1]);
}

void CgiProcessManager::setNonBlocking(int fileDescriptor) {
    const int flags = fcntl(fileDescriptor, F_GETFL, 0);
    fcntl(fileDescriptor, F_SETFL, flags | O_NONBLOCK);
}

void CgiProcessManager::setupParentPipes(const CgiPipes& pipes) {
    const int READING_PIPE_END = 0;
    const int WRITING_PIPE_END = 1;

    setNonBlocking(pipes.toProcess[WRITING_PIPE_END]);
    setNonBlocking(pipes.fromProcess[READING_PIPE_END]);
    setNonBlocking(pipes.control[READING_PIPE_END]);

    if (close(pipes.toProcess[READING_PIPE_END]) == -1 ||
        close(pipes.fromProcess[WRITING_PIPE_END]) == -1 ||
        close(pipes.control[WRITING_PIPE_END]) == -1) {
        throw runtime_error("close() failed on parent's pipe ends");
    }
}

void CgiProcessManager::writeRequestBodyToPipe(int pipeFd, const string& requestBody) {
    if (requestBody.empty()) {
        return;
    }

    size_t totalWritten = 0;
    while (totalWritten < requestBody.size()) {
        const ssize_t written =
            write(pipeFd, requestBody.data() + totalWritten, requestBody.size() - totalWritten);
        if (written == -1) {
            break;
        }
        totalWritten += written;
    }
}

void CgiProcessManager::runCgiChild(
    Listener* listener,
    int clientFd,
    int pipeToProcess[2],
    int pipeFromProcess[2],
    int controlPipe[2]
) {
    signal(SIGINT, SIG_DFL);
    signal(SIGTERM, SIG_DFL);
    signal(SIGQUIT, SIG_DFL);

    const int READING_PIPE_END = 0;
    const int WRITING_PIPE_END = 1;

    if (close(pipeToProcess[WRITING_PIPE_END]) == -1) {
        _log.stream(LOG_ERROR) << "close() failed on child's writing pipeToProcess end\n";
    }
    if (close(pipeFromProcess[READING_PIPE_END]) == -1) {
        _log.stream(LOG_ERROR) << "close() failed on child's reading pipeFromProcess end\n";
    }
    if (close(controlPipe[READING_PIPE_END]) == -1) {
        _log.stream(LOG_ERROR) << "close() failed on child's reading control pipe end\n";
    }

    if (dup2(pipeToProcess[READING_PIPE_END], STDIN_FILENO) == -1) {
        _log.stream(LOG_ERROR) << "dup2() failed for stdin in CGI child\n";
        char* argv[] = {
            const_cast<char*>("/bin/sh"),
            const_cast<char*>("-c"),
            const_cast<char*>("exit 1"),
            // clang-format off
            NULL};
        // clang-format on
        char* envp[] = {NULL};
        execve("/bin/sh", argv, envp);
        while (true) {
        }
    }
    if (close(pipeToProcess[READING_PIPE_END]) == -1) {
        _log.stream(LOG_ERROR) << "close() failed on child's pipeToProcess reading end\n";
    }

    if (dup2(pipeFromProcess[WRITING_PIPE_END], STDOUT_FILENO) == -1) {
        _log.stream(LOG_ERROR) << "dup2() failed for stdout in CGI child\n";
        char* argv[] = {
            const_cast<char*>("/bin/sh"),
            const_cast<char*>("-c"),
            const_cast<char*>("exit 1"),
            // clang-format off
            NULL};
        // clang-format on
        char* envp[] = {NULL};
        execve("/bin/sh", argv, envp);
        while (true) {
        }
    }
    if (close(pipeFromProcess[WRITING_PIPE_END]) == -1) {
        _log.stream(LOG_ERROR) << "close() failed on child's pipeFromProcess writing end\n";
    }

    Connection::State connState = listener->executeCgi(clientFd);

    if (write(controlPipe[WRITING_PIPE_END], &connState, sizeof(connState)) == -1) {
        _log.stream(LOG_ERROR) << "Failed to write to control pipe in CGI child\n";
    }
    if (close(controlPipe[WRITING_PIPE_END]) == -1) {
        _log.stream(LOG_ERROR) << "close() failed on child's control pipe writing end\n";
    }

    char* argv[] = {
        const_cast<char*>("/bin/sh"),
        const_cast<char*>("-c"),
        const_cast<char*>("exit 0"),
        // clang-format off
        NULL};
    // clang-format on
    char* envp[] = {NULL};
    execve("/bin/sh", argv, envp);
    while (true) {
    }
}

pid_t CgiProcessManager::startCgiProcess(
    Listener* listener,
    int clientFd,
    const string& requestBody,
    int& controlPipeReadEnd,
    int& responsePipeReadEnd
) {
    const int READING_PIPE_END = 0;
    const int WRITING_PIPE_END = 1;

    CgiPipes pipes = createPipes();

    const pid_t pid = fork();
    if (pid == -1) {
        closePipes(pipes);
        throw runtime_error("fork() failed for CGI");
    }

    if (pid == 0) {
        runCgiChild(listener, clientFd, pipes.toProcess, pipes.fromProcess, pipes.control);
    }

    setupParentPipes(pipes);
    writeRequestBodyToPipe(pipes.toProcess[WRITING_PIPE_END], requestBody);

    if (close(pipes.toProcess[WRITING_PIPE_END]) == -1) {
        _log.stream(LOG_ERROR) << "close() failed on parent's pipeToProcess writing end\n";
    }

    controlPipeReadEnd = pipes.control[READING_PIPE_END];
    responsePipeReadEnd = pipes.fromProcess[READING_PIPE_END];

    return (pid);
}

void CgiProcessManager::parseCgiResponseLoop(
    const string& headers,
    int& statusCode,
    string& contentType,
    map<string, string>& customHeaders
) {
    std::istringstream headerStream(headers);
    string line;

    statusCode = HttpStatus::OK;
    contentType = "text/html";
    customHeaders.clear();

    while (!std::getline(headerStream, line).fail()) {
        if (!line.empty() && line[line.size() - 1] == '\r') {
            line.erase(line.size() - 1);
        }

        const string::size_type colon = line.find(':');
        if (colon == string::npos) {
            continue;
        }

        const string key = line.substr(0, colon);
        string value = line.substr(colon + 1);

        while (!value.empty() && value[0] == ' ') {
            value.erase(0, 1);
        }

        if (key == "Status") {
            std::istringstream statusStream(value);
            statusStream >> statusCode;
        } else if (key == "Content-Type" || key == "Content-type") {
            contentType = value;
        } else {
            customHeaders[key] = value;
        }
    }
}

string CgiProcessManager::parseCgiResponse(const string& cgiOutput) {
    const string::size_type headerEnd = cgiOutput.find("\r\n\r\n");
    if (headerEnd == string::npos) {
        const Response response(HttpStatus::OK, cgiOutput, "text/html");
        return (response.serialize());
    }

    const string headers = cgiOutput.substr(0, headerEnd);
    const string body = cgiOutput.substr(headerEnd + 4);

    int statusCode = HttpStatus::OK;
    string contentType = "text/html";
    map<string, string> customHeaders;

    parseCgiResponseLoop(headers, statusCode, contentType, customHeaders);

    Response response(statusCode, body, contentType);

    for (map<string, string>::const_iterator iter = customHeaders.begin();
         iter != customHeaders.end();
         ++iter) {
        response.setHeader(iter->first, iter->second);
    }

    return (response.serialize());
}

void CgiProcessManager::registerWorker(int clientFd, pid_t pid) {
    _cgiWorkers.insert(clientFd);
    _cgiProcesses[clientFd] = pid;
    _cgiStartTimes[clientFd] = time(NULL);
    _cgiTimeouts[clientFd] = DEFAULT_CGI_TIMEOUT_SECONDS;
}

bool CgiProcessManager::isWorker(int clientFd) const {
    return (_cgiWorkers.find(clientFd) != _cgiWorkers.end());
}

void CgiProcessManager::unregisterWorker(int clientFd) {
    _cgiWorkers.erase(clientFd);
}

void CgiProcessManager::cleanupProcess(int clientFd) {
    _cgiProcesses.erase(clientFd);
    _cgiStartTimes.erase(clientFd);
    _cgiTimeouts.erase(clientFd);
    _cgiWorkers.erase(clientFd);
}

pid_t CgiProcessManager::getProcessId(int clientFd) const {
    const map<int, pid_t>::const_iterator iter = _cgiProcesses.find(clientFd);
    if (iter == _cgiProcesses.end()) {
        return (-1);
    }
    return (iter->second);
}

vector<int> CgiProcessManager::checkTimeouts() {
    const time_t now = time(NULL);
    vector<int> timedOutFds;

    for (map<int, time_t>::iterator iter = _cgiStartTimes.begin(); iter != _cgiStartTimes.end();
         ++iter) {
        const int clientFd = iter->first;
        const time_t startTime = iter->second;

        int timeoutSeconds = DEFAULT_CGI_TIMEOUT_SECONDS;
        const map<int, int>::iterator timeoutIt = _cgiTimeouts.find(clientFd);
        if (timeoutIt != _cgiTimeouts.end()) {
            timeoutSeconds = timeoutIt->second;
        }

        if (now - startTime > timeoutSeconds) {
            _log.stream(LOG_WARN) << "CGI timeout exceeded for client " << clientFd
                                  << " (running for " << (now - startTime) << " seconds)\n";

            const map<int, pid_t>::iterator pidIt = _cgiProcesses.find(clientFd);
            if (pidIt != _cgiProcesses.end()) {
                const pid_t pid = pidIt->second;
                _log.stream(LOG_WARN) << "Killing CGI process " << pid << "\n";
                kill(pid, SIGKILL);
                timedOutFds.push_back(clientFd);
            }
        }
    }

    return (timedOutFds);
}

}  // namespace webserver
