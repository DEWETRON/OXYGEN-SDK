// Copyright (c) DEWETRON GmbH 2017
#ifndef __ODKBASE_IFVALUE__
#define __ODKBASE_IFVALUE__

#include "odkbase_if_api_object.h"

/**
 * This header contains interfaces for the value objects used in the plugin API.
 * All of these objects are reference counted.
 * Functions returning such objects already increment the reference counter before returning the object.
 * Functions accepting values increment the reference count if the instance is required after returning from the call.
 * Notable exceptions, that reduced reference counting overhead for specific usage scenarios:
 *   - Methods starting with "ref" return a reference to an internal object with out incrementing the reference count.
 *     Therefore clients are not forced to release() such instances if they only need them for a very short time.
 *     Referenced values will become invalid if the owner value changes or is destroyed.
 *   - Methods called adopt (or methods called with the adopt flag set to true) transfer ownership of the instance from caller to callee.
 *     Therefore the caller does not have to release() his reference after the call.
 */

namespace odk
{

    class IfValue : public IfApiObject
    {
    public:
        enum class Type : int
        {
            TYPE_INVALID            = 0x00,
            TYPE_ERROR              = 0x01, //Indicates an error while creating this value (error in function,...)

            VALID_TYPES_START       = 0x05, //compare x < VALID_TYPES_START to check for any proper value

            TYPE_STRING             = 0x10,
            TYPE_XML                = 0x11,

            TYPE_BOOL               = 0x20,
            TYPE_SINT               = 0x21,
            TYPE_UINT               = 0x22,
            TYPE_FLOAT              = 0x23,
            TYPE_COMPLEX            = 0x24,
            TYPE_POINT              = 0x25,

            TYPE_SCALAR             = 0x30,
            TYPE_RATIONAL_SCALAR    = 0x31,
            TYPE_DECORATED_NUMBER   = 0x32,
            TYPE_RANGE              = 0x33,

            TYPE_ENUM               = 0x40,
            TYPE_ARBITRARY_DATA     = 0x42,

            TYPE_DATA_BLOCK         = 0x43,
            TYPE_DATA_BLOCK_LIST    = 0x44,

            TYPE_CHANNEL_ID         = 0x45,

            //Types that are lists of values
            TYPE_STRING_VECTOR      = 0x0100 | TYPE_STRING,
            TYPE_FLOAT_VECTOR       = 0x0100 | TYPE_FLOAT,
            TYPE_COMPLEX_VECTOR     = 0x0100 | TYPE_COMPLEX,
            TYPE_POINT_VECTOR       = 0x0100 | TYPE_POINT,
            TYPE_CHANNEL_ID_VECTOR  = 0x0100 | TYPE_CHANNEL_ID,
        };

        virtual Type PLUGIN_API getType() const = 0;

        /**
         * Returns a descriptive representation of the current content.
         * This string is not intended to be serialized except for logging purposes.
         * The memory region will remain valid until
         * - the value of this config item is changed by the client, or
         * - the config item object is destroyed by the client.
         */
        virtual const char* PLUGIN_API getDebugString() const = 0;

    protected:
        IfValue() = default;
        ~IfValue() = default; // no virtual destructor to keep the vtable clean
    };

    template <class T, class O>
    T* value_cast(O* ptr)
    {
        T* ret = nullptr;
        if (ptr && ptr->getType() == T::type_index)
        {
            ret = static_cast<T*>(ptr);
        }
        return ret;
    }

    template <class T, class O>
    const T* value_cast(const O* ptr)
    {
        const T* ret = nullptr;
        if (ptr && ptr->getType() == T::type_index)
        {
            ret = static_cast<const T*>(ptr);
        }
        return ret;
    }

}

#endif //__ODKBASE_IFVALUE__
