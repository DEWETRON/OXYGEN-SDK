// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkapi_data_set_descriptor_xml.h"
#include "odkuni_defines.h"
#include <cstddef>
#include <cstdint>

namespace odk
{
namespace framework
{
    class BlockIterator
    {
    public:
        BlockIterator() noexcept;
        BlockIterator(const void* data, std::size_t data_stride, std::uint64_t initial_timestamp) noexcept;
        BlockIterator(const void* data, std::size_t data_stride, const std::uint64_t* timestamp, std::size_t timestamp_stride) noexcept;
        BlockIterator(const void* data, std::size_t data_stride, const std::uint64_t* timestamp, std::size_t timestamp_stride, const std::uint32_t* sample_size, std::size_t sample_size_stride) noexcept;
        BlockIterator(std::uint64_t timestamp) noexcept;

        /// Start address of the sample
        ODK_NODISCARD inline const void* data() const noexcept { return m_data; }
        /// Timestamp of the sample
        ODK_NODISCARD inline std::uint64_t timestamp() const noexcept { return m_timestamp ? *m_timestamp : m_timestamp_value;}
        /// Dynamic sample size of the sample (0 if it has a static size)
        ODK_NODISCARD inline std::size_t size() const noexcept { return m_sample_size ? *m_sample_size : m_sample_size_value; }

        BlockIterator& operator++();
        BlockIterator& operator--();

        ODK_NODISCARD inline bool operator==(const BlockIterator& other) const noexcept
        {
            return m_data && other.m_data ?
                          (m_data == other.m_data)
                          : timestamp() == other.timestamp();
        }

        ODK_NODISCARD inline bool operator!=(const BlockIterator& other) const noexcept
        {
            return !(*this == other);
        }

        ODK_NODISCARD inline bool operator<(const BlockIterator& other) const noexcept
        {
            return timestamp() < other.timestamp();
        }

        ODK_NODISCARD inline bool operator<=(const BlockIterator& other) const noexcept
        {
            return timestamp() <= other.timestamp();
        }

        ODK_NODISCARD inline bool operator>=(const BlockIterator& other) const noexcept
        {
            return timestamp() >= other.timestamp();
        }

        ODK_NODISCARD std::uint64_t distanceTo(const BlockIterator& other) const noexcept;

    private:
        const void* m_data;
        std::size_t m_data_stride;
        const std::uint64_t* m_timestamp;
        union
        {
            std::size_t   m_timestamp_stride;
            std::uint64_t m_timestamp_value;
        };

        const std::uint32_t* m_sample_size;
        union
        {
            std::size_t m_sample_size_stride;
            std::size_t m_sample_size_value;
        };
    };
}
}
