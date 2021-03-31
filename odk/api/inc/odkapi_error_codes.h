// Copyright DEWETRON GmbH 2019

#ifndef __ODKAPI_ERROR_CODES__
#define __ODKAPI_ERROR_CODES__

#include <cstdint>

#define BASE_ID(name, value) const static std::uint64_t name = value;
#define ERROR_ID(name, base, value, desc) \
    const static std::uint64_t name = error_base::base + value; \
    static const char* name ## _Description = desc;

namespace odk
{
namespace error_base
{
    // 0x00000000... Generic error
    BASE_ID(GENERIC,                       0x0000000000000000ull);

    // 0x000F.... command specific error codes
    BASE_ID(COMMAND_SPECIFIC,              0x000F000000000000ull);

    // 0xF.... special error codes
}

#define ADD_NAMED_ERROR(name) \
    case name: \
        return name ## _Description;

namespace error_codes
{
    // NOTE: the description strings should match those in the translation file "oxygen_exceptions_en.ts", context PLUGIN_API
    ERROR_ID(NOT_IMPLEMENTED,          GENERIC, 0xffffffffffffffff, "The requested functionality is not implemented by the receiver.");
    ERROR_ID(OXYNET_NODE_DISABLED,     GENERIC, 0xfffffffffffffffe, "The Oxynet node is disabled");

    ERROR_ID(OK,                       GENERIC, 0x00000000, "No error");
    ERROR_ID(INVALID_INPUT_PARAMETER,  GENERIC, 0x00000001, "An input parameter is invalid");
    ERROR_ID(INVALID_OUTPUT_PARAMETER, GENERIC, 0x00000002, "An output parameter is invalid");
    ERROR_ID(INVALID_MODE,             GENERIC, 0x00000003, "The operation is not supported in the current oxygen mode");
    ERROR_ID(INTERNAL_ERROR,           GENERIC, 0x00000004, "Internal error");
    ERROR_ID(UNSUPPORTED_CHANNEL_TYPE, GENERIC, 0x00000005, "Unsupported channel type");
    ERROR_ID(IO_ERROR,                 GENERIC, 0x00000006, "I/O error");
    ERROR_ID(INVALID_CHANNEL,          GENERIC, 0x00000007, "Invalid channel");
    ERROR_ID(INVALID_OBJECT,           GENERIC, 0x00000008, "The requested object was not found");
    ERROR_ID(DUPLICATE_CHANNEL,        GENERIC, 0x00000009, "Duplicate channel");
    ERROR_ID(CONFIGURATION_CHANGED,    GENERIC, 0x0000000A, "The configuration has changed");
    ERROR_ID(INVALID_STATE,            GENERIC, 0x0000000B, "Invalid state");
    ERROR_ID(UNSUPPORTED_VERSION,      GENERIC, 0x0000000C, "Unsupported version");
    ERROR_ID(UNHANDLED_EXCEPTION,      GENERIC, 0x0000000D, "Unhandled exception");


    ERROR_ID(COMMAND_SPECIFIC_RETURN_BASE, COMMAND_SPECIFIC, 0x00000000, "Commands can use up to 2^48 custom return codes starting at this id");

    inline const char* toString(std::uint64_t code)
    {
        switch (code)
        {
            ADD_NAMED_ERROR(OK);
            ADD_NAMED_ERROR(INVALID_INPUT_PARAMETER);
            ADD_NAMED_ERROR(INVALID_OUTPUT_PARAMETER);
            ADD_NAMED_ERROR(INVALID_MODE);
            ADD_NAMED_ERROR(INTERNAL_ERROR);
            ADD_NAMED_ERROR(UNSUPPORTED_CHANNEL_TYPE);
            ADD_NAMED_ERROR(IO_ERROR);
            ADD_NAMED_ERROR(INVALID_CHANNEL);
            ADD_NAMED_ERROR(INVALID_OBJECT);
            ADD_NAMED_ERROR(DUPLICATE_CHANNEL);
            ADD_NAMED_ERROR(CONFIGURATION_CHANGED);
            ADD_NAMED_ERROR(OXYNET_NODE_DISABLED);
            ADD_NAMED_ERROR(INVALID_STATE);
            ADD_NAMED_ERROR(UNSUPPORTED_VERSION);
            ADD_NAMED_ERROR(UNHANDLED_EXCEPTION);
            default:
                return nullptr;
        }
    }
}
}

#undef ADD_NAMED_ERROR
#undef BASE_ID
#undef ERROR_ID

#endif
