#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <ostream>
#include <string>

enum LogLevel { LOG_SILENT, LOG_TRACE, LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR, LOG_FATAL };

namespace webserver {

class Logger {
public:
    Logger();
    explicit Logger(LogLevel level);
    ~Logger();

    void setLocalLevel(LogLevel level);
    void clearLocalLevel();

    std::ostream& stream(LogLevel level);

private:
    Logger(const Logger& other);
    Logger& operator=(const Logger& other);

    bool _hasLocalLevel;
    LogLevel _localLevel;

    LogLevel getEffectiveLevel() const;

    static std::string levelToString(LogLevel level);
    static const char* logLevelToColor(LogLevel level);
};

}  // namespace webserver

#endif
