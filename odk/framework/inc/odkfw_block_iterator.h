// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkapi_data_set_descriptor_xml.h"
#include <cstddef>
#include <cstdint>

namespace odk
{
namespace framework
{
    class BlockIterator
    {
    public:
        BlockIterator();
        BlockIterator(const void* data, std::size_t data_stride, std::uint64_t initial_timestamp);
        BlockIterator(const void* data, std::size_t data_stride, const std::uint64_t* timestamp, std::size_t timestamp_stride);
        BlockIterator(const std::uint64_t timestamp);

        /// Start address of the sample
        inline const void* data() const { return m_data; }
        /// Timestamp of the sample
        inline std::uint64_t timestamp() const { return m_timestamp_value;}

        inline BlockIterator& operator++()
        {
            if (m_data)
            {
                m_data = reinterpret_cast<const std::uint8_t*>(m_data) + m_data_stride;
            }

            if (m_timestamp)
            {
                m_timestamp = reinterpret_cast<const std::uint64_t*>(
                    reinterpret_cast<const std::uint8_t*>(m_timestamp) + m_timestamp_stride);
                m_timestamp_value = *m_timestamp;
            }
            else
            {
                ++m_timestamp_value;
            }
            return *this;
        }

        inline BlockIterator& operator--()
        {
            if (m_data)
            {
                m_data = reinterpret_cast<const std::uint8_t*>(m_data) - m_data_stride;
            }

            if (m_timestamp)
            {
                m_timestamp = reinterpret_cast<const std::uint64_t*>(
                    reinterpret_cast<const std::uint8_t*>(m_timestamp) - m_timestamp_stride);
                m_timestamp_value = *m_timestamp;
            }
            else
            {
                --m_timestamp_value;
            }
            return *this;
        }

        inline bool operator==(const BlockIterator& other) const
        {
            return m_data && other.m_data ?
                          (m_data == other.m_data)
                          : timestamp() == other.timestamp();
        }

        inline bool operator!=(const BlockIterator& other) const
        {
            return !(*this == other);
        }

        inline bool operator<(const BlockIterator& other) const
        {
            return timestamp() < other.timestamp();
        }

        inline bool operator<=(const BlockIterator& other) const
        {
            return timestamp() <= other.timestamp();
        }

        inline bool operator>=(const BlockIterator& other) const
        {
            return timestamp() >= other.timestamp();
        }

    private:
        const void* m_data;
        std::size_t m_data_stride;
        const std::uint64_t* m_timestamp;
        std::size_t   m_timestamp_stride;
        std::uint64_t m_timestamp_value;
    };
}
}
