// Copyright DEWETRON GmbH 2017

#ifndef __ODKBASE_IF_HOST__
#define __ODKBASE_IF_HOST__

#include "odkbase_if_host_fwd.h"
#include "odkbase_if_value.h"

#ifdef ODK_EXTENSION_FUNCTIONS
#include "odkbase_api_object_ptr.h"
#endif

#include "odkuni_defines.h"

#include <cstdint>

namespace odk
{
    /**
     * Interface to communicate with the host application.
     */
    class IfHost
    {
    public:

        /**
         * Creates an empty and mutable value object of the specified type.
         * All instances of type IfValue that are used in the API have to be created using this method!
         * @param  type Select the kind of value that should be created. @see odk::IfValue::Type for supported constants.
         * @return      A new, mutable value instance of the request type with reference count set to 1 (or nullptr if the call failed)
         */
        ODK_NODISCARD virtual IfValue* PLUGIN_API createValue(IfValue::Type type) const = 0;

        /**
         * Generic function for sending predefined messages to the host and receive a synchronous result.
         * @param  msg_id Describes the type and semantics of the message.
         *                Check piapi_message_ids.h, namespace "host_msg" for valid values and more information about each message.
         * @param  key    Meaning of this parameter depends on the message type. Can be used to select which instance (of many) the message targets.
         * @param  param  Meaning and type of this parameter depends on the message type.
         * @param  ret    Meaning and type of the value assigned to this
         *                parameter depends on the message type (OUT param only, will not be read).
         * @return        Meaning depends on the message type. Returns (uint64)-1 for unknown or not implemented messages.
         */
        virtual std::uint64_t PLUGIN_API messageSync(MessageId msg_id, std::uint64_t key, const IfValue* param, const IfValue** ret = nullptr) = 0;

        /**
         * Generic function for sending predefined messages to the host and receive a synchronous result.
         * This version avoids the overhead of creating and initializing an IfValue instance and directly reads from the supplied memory location.
         * Plese check the documentation to see which messages support this function and how the memory is interpreted.
         * @param  msg_id Describes the type and semantics of the message.
         *                Check piapi_message_ids.h, namespace "host_msg" for valid values and more information about each message.
         * @param  key    Meaning of this parameter depends on the message type. Can be used to select which instance (of many) the message targets.
         * @param  param       Pointer to a data buffer that has to remain unchanged until the call is completed.
         * @param  param_size  Size of the data buffer in bytes. If buffer contains string or xml data this has to include the 0-termination character.
         * @param  ret    Meaning and type of the value assigned to this
         *                parameter depends on the message type (OUT param only, will not be read).
         * @return        Meaning depends on the message type. Returns (uint64)-1 for unknown or not implemented messages.
         */
        virtual std::uint64_t PLUGIN_API messageSyncData(MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const IfValue** ret = nullptr) = 0;

        /**
         * Generic function for sending predefined messages to the host in an asynchronous way without a return value.
         * @param  msg_id Describes the type and semantics of the message.
         *                Check piapi_message_ids.h, namespace "host_msg_async" for valid values and more information about each message.
         * @param  key    Meaning of this parameter depends on the message type. Can be used to select which instance (of many) the message targets.
         * @param  param  Meaning and type of this parameter depends on the message type.
         * @return        Meaning depends on the message type. Returns (uint64)-1 for unknown or not implemented messages.
         */
        virtual std::uint64_t PLUGIN_API messageAsync(MessageId msg_id, std::uint64_t key, const IfValue* param) = 0;

        /**
         * Generic function to get/set properties or invoke specific functionality.
         * @param  context Used together with item to select the target of the call.
         * @param  item    Used together with context to select the target of the call.
         * @param  param   Meaning and type of this parameter depends on the selected target.
         * @return         Detailed meaning and type depends on the selected target.
         *                 In general, an instance of IfErrorValue is returned to indicate a problem when executing the query.
         *                 Queries that have no defined return value (e.g. property assignment) return nullptr if performed successfully.
         */
        virtual const IfValue* PLUGIN_API query(const char* context, const char* item, const IfValue* param) = 0;

        /**
         * Optimized version of query that directly accepts an XML document as parameter.
         * Identical to calling query() with an XMLValue containing the same text, but avoids allocation of a new buffer just to transfer the value.
         * @see IfHost::query
         * @param xml_size length of the supplied xml document including the 0-termination character.
         */
        virtual const IfValue* PLUGIN_API queryXML(const char* context, const char* item, const char* xml, std::uint64_t xml_size) = 0;

    protected:
        ~IfHost() = default; // no virtual destructor to keep the vtable clean


#ifdef ODK_EXTENSION_FUNCTIONS
    public: // Extension methods; only available to C++

        /**
         * Same functionality as IfHost::createValue(type), but returns a properly casted value wrapped in a smart pointer.
         */
        template <class T>
        ODK_NODISCARD detail::ApiObjectPtr<T> createValue()
        {
            auto val = odk::ptr(createValue(T::type_index));
            return odk::api_ptr_cast<T>(val);
        }

        /**
         * Same functionality as IfHost::query() but returns the value converted to the expected type.
         * @return Valid smart pointer to the return value iff query was successful and return value matched the expected type; or an empty pointer.
         */
        template <class T>
        detail::ApiObjectPtr<const T> getValue(const char* context, const char* item)
        {
            auto val = odk::ptr(query(context, item, nullptr));
            if (!val)
            {
                return {};
            }
            if (val->getType() == T::type_index)
            {
                return odk::api_ptr_cast<const T>(val);
            }
            return {};
        }
#endif
    };

}


#endif //__ODKBASE_IF_HOST__

