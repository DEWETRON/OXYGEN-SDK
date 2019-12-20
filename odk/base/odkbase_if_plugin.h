// Copyright DEWETRON GmbH 2017

#ifndef __PIAPI_IF_PLUGIN__
#define __PIAPI_IF_PLUGIN__

#include "odkbase_if_api_object.h"
#include "odkuni_uuid.h"

#include <cstdint>

namespace odk
{
    class IfHost;
    class IfValue;

    typedef std::uint64_t PluginMessageId;

    /**
     * Interface that has to be implemented by all plugins.
     */
    class IfPlugin
    {
    public:

        /**
         * This is invoked immediately after creation of the plugin to provide an interface to the hosting application.
         */
        virtual void PLUGIN_API setPluginHost(IfHost* host) = 0;

        /**
         * Plugins have to implement this function to process requests from the host application.
         * @param  msg_id Describes the type and semantics of the message.
         *                Check piapi_message_ids.h, namespace "plugin_msg_sync" for valid values and more information about each message.
         * @param  key    Meaning of this parameter depends on the message type. Can be used to select which instance (of many) the message targets.
         * @param  param  Meaning and type of this parameter depends on the message type.
         * @param  ret    Meaning and type of the value assigned to this parameter depends on the message type.
         * @return        Meaning depends on the message type.
         *                Implementations have to return (uint64)-1 for unknown or not implemented messages.
         */
        virtual std::uint64_t PLUGIN_API pluginMessage(PluginMessageId msg_id, std::uint64_t key, const IfValue* param, const IfValue** ret) = 0;

    protected:
        ~IfPlugin() = default; // no virtual destructor to keep the vtable clean
    };

    //UUID for plugins that are to be used in Oxygen, which have to implement the IfPlugin interface.
    static const odk::UuidType Oxygen_Plugin_Uuid =
      odk::generateUuidType(0xd1, 0x7e, 0x33, 0x04, 0x9b, 0x11, 0x4e, 0x34, 0xa9, 0x31, 0x3f, 0xfa, 0x92, 0x3d, 0x1b, 0x98);
      // {D17E3304-9B11-4E34-A931-3FFA923D1B98}

}


#endif //__PIAPI_IF_PLUGIN__

