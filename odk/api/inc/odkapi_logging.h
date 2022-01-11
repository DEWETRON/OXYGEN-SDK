// Copyright DEWETRON GmbH 2022

#pragma once

namespace odk
{
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
}
