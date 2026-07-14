// Copyright DEWETRON GmbH 2026

#define ODK_EXTENSION_FUNCTIONS //enable C++ integration

#include "odkapi_logging.h"
#include "odkapi_message_ids.h"
#include "odkbase_basic_values.h"
#include "odkbase_if_host.h"

void odk::logMessage(odk::IfHost* host, LogSeverityLevel log_level, const char* message)
{
    auto log_msg = host->createValue<odk::IfStringValue>();
    log_msg->set(message);
    host->messageAsync(odk::host_msg_async::LOG_MESSAGE, log_level, log_msg.get());
}
