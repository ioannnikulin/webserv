#include "LoggerConfig.hpp"

#include "Logger.hpp"

namespace webserver {

LogLevel LoggerConfig::_globalLevel = LOG_TRACE;
bool LoggerConfig::_includeLevel = true;
bool LoggerConfig::_includeTimestamp = true;

void LoggerConfig::setGlobalLevel(LogLevel level) {
    _globalLevel = level;
}

LogLevel LoggerConfig::getGlobalLevel() {
    return (_globalLevel);
}

bool LoggerConfig::getIncludeLevel() {
    return (_includeLevel);
}

void LoggerConfig::setIncludeLevel(bool value) {
    _includeLevel = value;
}

bool LoggerConfig::getIncludeTimestamp() {
    return (_includeTimestamp);
}

void LoggerConfig::setIncludeTimestamp(bool value) {
    _includeTimestamp = value;
}

}  // namespace webserver
