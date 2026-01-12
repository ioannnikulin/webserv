#include "Logger.hpp"

#include <iostream>
#include <string>

#include "LoggerConfig.hpp"
#include "utils/colors.hpp"
#include "utils/utils.hpp"

#define LOG_LEVEL_STR_WIDTH 5

namespace {
struct NullBuf : std::streambuf {
protected:
    int overflow(int value) {
        return (value);
    }
};

std::ostream& getNullStream() {
    static NullBuf buf;
    static std::ostream stream(&buf);
    return (stream);
}
}  // namespace

namespace webserver {

Logger::Logger()
    : _hasLocalLevel(false)
    , _localLevel(DEFAULT_LOG_LEVEL) {
}

Logger::Logger(LogLevel level)
    : _hasLocalLevel(true)
    , _localLevel(level) {
}

Logger::~Logger() {
}

void Logger::setLocalLevel(LogLevel level) {
    _hasLocalLevel = true;
    _localLevel = level;
}

void Logger::clearLocalLevel() {
    _hasLocalLevel = false;
}

LogLevel Logger::getEffectiveLevel() const {
    return (_hasLocalLevel ? _localLevel : LoggerConfig::getGlobalLevel());
}

std::ostream& Logger::stream(LogLevel level) {
    std::ostream* stream = &std::cout;

    const int lvl = static_cast<int>(level);

    if (lvl == LOG_SILENT || getEffectiveLevel() > lvl) {
        return (getNullStream());
    }
    if (lvl < static_cast<int>(LOG_INFO)) {
        stream = &std::clog;
    } else if (lvl > static_cast<int>(LOG_WARN)) {
        stream = &std::cerr;
    }

    const std::string lvlStr = levelToString(level);

    (*stream) << logLevelToColor(level);

    if (LoggerConfig::getIncludeLevel()) {
        (*stream) << "[" << lvlStr << "] ";
    }

    if (lvlStr.size() < LOG_LEVEL_STR_WIDTH) {
        (*stream) << " ";
    }

    if (LoggerConfig::getIncludeTimestamp()) {
        (*stream) << utils::getTimestamp() << " | ";
    }
    return (*stream);
}

std::string Logger::levelToString(LogLevel level) {
    /* NOTE: log level explanations
    silent - no output; used for tests or performance-critical runs
    trace  - entering a function, printing a variable value
    debug  - decisions ("this was a GET request", "FSM in READING_COMPLETE now", "parsing results")
    info   - external events (start/stop, connected, sent response and its contents)
    warn   - server would prefer this to be different, but recovered and the standard process is going on
    error  - standard process broken
    fatal  - exiting
    */
    switch (level) {
        case LOG_SILENT:
            return ("SILENT");
        case LOG_TRACE:
            return ("TRACE");
        case LOG_DEBUG:
            return ("DEBUG");
        case LOG_INFO:
            return ("INFO");
        case LOG_WARN:
            return ("WARN");
        case LOG_ERROR:
            return ("ERROR");
        case LOG_FATAL:
            return ("FATAL");
    }
    return ("UNKNOWN");
}

const char* Logger::logLevelToColor(LogLevel level) {
    switch (level) {
        case LOG_SILENT:
            return (WHITE);
        case LOG_TRACE:
            return (C_TRACE);
        case LOG_DEBUG:
            return (C_DEBUG);
        case LOG_INFO:
            return (C_INFO);
        case LOG_WARN:
            return (C_WARN);
        case LOG_ERROR:
            return (C_ERROR);
        case LOG_FATAL:
            return (C_FATAL);
    }
    return (C_RESET);
}

}  // namespace webserver
