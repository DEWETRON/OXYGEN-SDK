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

        /// Start address of the sample
        const void* data() const { return m_data; }
        /// Timestamp of the sample
        std::uint64_t timestamp() const { return m_timestamp ? *m_timestamp : m_timestamp_value; }

        BlockIterator& operator++();
        BlockIterator& operator--();
        bool operator==(const BlockIterator&) const;
        bool operator!=(const BlockIterator&) const;

    private:
        const void* m_data;
        std::size_t m_data_stride;
        const std::uint64_t* m_timestamp;
        union
        {
            std::size_t   m_timestamp_stride;
            std::uint64_t m_timestamp_value;
        };
    };
}
}
