#ifndef LOGGERCONFIG_HPP
#define LOGGERCONFIG_HPP

// NOTE: DL: I wanted to move all settings to a separate setting file but doing this through macros is seemingly a bad idea
#define DEFAULT_LOG_LEVEL LOG_INFO
#define GLOBAL_LOG_LEVEL LOG_INFO

#define C_RESET RESET_COLOR
#define C_TRACE GREY
#define C_DEBUG CYAN
#define C_INFO GREEN
#define C_WARN YELLOW
#define C_ERROR ORANGE
#define C_FATAL B_RED

#include "Logger.hpp"

namespace webserver {
class LoggerConfig {
private:
    LoggerConfig();
    LoggerConfig(const LoggerConfig&);
    LoggerConfig& operator=(const LoggerConfig&);
    ~LoggerConfig();

    static LogLevel _globalLevel;
    static bool _includeLevel;
    static bool _includeTimestamp;

public:
    static LogLevel getGlobalLevel();
    static void setGlobalLevel(LogLevel level);

    static bool getIncludeLevel();
    static void setIncludeLevel(bool value);

    static bool getIncludeTimestamp();
    static void setIncludeTimestamp(bool value);
};

}  // namespace webserver

#endif
