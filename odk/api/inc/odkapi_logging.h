// Copyright DEWETRON GmbH 2022

#pragma once

namespace odk
{
    class IfHost;

    /**
     * Log Severity level that is provided as the <key> argument in sync/async LOG_MESSAGE messages
     */
    enum LogSeverityLevel
    {
        LOGLEVEL_FATAL = 1,
        LOGLEVEL_ERROR = 2,
        LOGLEVEL_WARNING = 3,
        LOGLEVEL_INFO = 4,
        LOGLEVEL_TRACE = 5,
        LOGLEVEL_DEBUG = 6,
    };

    /**
     * Write a message to the log file
     *
     * @param host      pointer to host interface
     * @param log_level level as defined in odkapi_logging.h
     * @param message   message string
     */
    void logMessage(odk::IfHost* host, LogSeverityLevel log_level, const char* message);
}
