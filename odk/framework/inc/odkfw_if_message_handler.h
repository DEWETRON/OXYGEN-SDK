// Copyright DEWETRON GmbH 2019
#pragma once

#include "odkbase_api_object_ptr.h"
#include "odkbase_if_value.h"
#include "odkbase_if_plugin.h"
#include "odkapi_error_codes.h"
#include "odkapi_message_ids.h"
#include "odkuni_defines.h"

#include <cstdint>

namespace odk
{
namespace framework
{

    class IfMessageHandler
    {
    public:
        virtual std::uint64_t pluginMessage(
            odk::PluginMessageId id,
            std::uint64_t key,
            const odk::IfValue* param,
            const odk::IfValue** ret) = 0;

        virtual void setHost(odk::IfHost* host) { ODK_UNUSED(host); };

    protected:
        virtual ~IfMessageHandler()
        {}
    };

}
}

