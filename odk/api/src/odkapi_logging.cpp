// Copyright DEWETRON GmbH 2026

#define ODK_EXTENSION_FUNCTIONS //enable C++ integration

#include "odkapi_logging.h"
#include "odkapi_message_ids.h"
#include "odkbase_basic_values.h"
#include "odkbase_if_host.h"

#include <cstdarg>
#include <cstdio>
#include <string>

void odk::logMessage(odk::IfHost* host, LogSeverityLevel log_level, const char* format, ...)
{
    if (!host)
    {
        return;
    }

    auto log_msg = host->createValue<odk::IfStringValue>();
    if (!log_msg)
    {
        return;
    }

    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);
    int chars_needed = std::vsnprintf(nullptr, 0, format, args_copy);
    va_end(args_copy);

    std::string formatted;
    if (chars_needed > 0)
    {
        formatted.resize(static_cast<size_t>(chars_needed));
        std::vsnprintf(formatted.data(), formatted.size() + 1, format, args);
        log_msg->set(formatted.c_str());
    }

    va_end(args);

    host->messageAsync(odk::host_msg_async::LOG_MESSAGE, log_level, log_msg.get());
}
