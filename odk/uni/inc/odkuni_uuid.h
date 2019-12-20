// Copyright (c) DEWETRON GmbH 2015
#pragma once

#define ODK_UUID_SIZE 16

#include <cstdint>
#include <cstring>

namespace odk
{
    /**
     * An UUID is a 128bit number split into 16 8bit characters.
     */
    struct UuidType
    {
        std::uint8_t    m_bytes[ODK_UUID_SIZE];

        UuidType()
        {
            std::memset(m_bytes, 0, ODK_UUID_SIZE);
        }

        explicit UuidType(const void* src)
        {
            std::memcpy(m_bytes, src, ODK_UUID_SIZE);
        }

        bool operator==(const UuidType& rhs) const
        {
            return std::memcmp(m_bytes, rhs.m_bytes, sizeof(m_bytes)) == 0;
        }

        bool operator!=(const UuidType& rhs) const
        {
            return !(*this == rhs);
        }

        bool operator<(const UuidType& rhs) const
        {
            return std::memcmp(m_bytes, rhs.m_bytes, sizeof(m_bytes)) < 0;
        }
    };

    inline UuidType generateUuidType(std::uint8_t u0, std::uint8_t u1, std::uint8_t u2, std::uint8_t u3,
                                     std::uint8_t u4, std::uint8_t u5, std::uint8_t u6, std::uint8_t u7,
                                     std::uint8_t u8, std::uint8_t u9, std::uint8_t u10, std::uint8_t u11,
                                     std::uint8_t u12, std::uint8_t u13, std::uint8_t u14, std::uint8_t u15)
    {
        const std::uint8_t bytes[] = { u0, u1, u2, u3, u4, u5, u6, u7, u8, u9, u10, u11, u12, u13, u14, u15 };
        UuidType raw(bytes);
        return raw;
    }

} //odk


