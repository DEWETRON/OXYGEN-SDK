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
        BlockIterator(const void* data, std::size_t data_stride, const std::uint64_t* timestamp, std::size_t timestamp_stride, const std::uint32_t* sample_size, std::size_t sample_size_stride);
        BlockIterator(const std::uint64_t timestamp);

        /// Start address of the sample
        inline const void* data() const { return m_data; }
        /// Timestamp of the sample
        inline std::uint64_t timestamp() const { return m_timestamp_value;}
        
        inline std::size_t size() const { return m_sample_size_value; }

        inline BlockIterator& operator++()
        {
            if (m_data)
            {
                m_data = reinterpret_cast<const std::uint8_t*>(m_data) + m_data_stride + m_sample_size_value;
            }

            if (m_timestamp)
            {
                m_timestamp = reinterpret_cast<const std::uint64_t*>(
                    reinterpret_cast<const std::uint8_t*>(m_timestamp) + m_timestamp_stride + m_sample_size_value);
                m_timestamp_value = *m_timestamp;
            }
            else
            {
                ++m_timestamp_value;
            }

            if (m_sample_size)
            {
                m_sample_size = reinterpret_cast<const std::uint32_t*>(
                    reinterpret_cast<const std::uint8_t*>(m_sample_size) + m_sample_size_stride + m_sample_size_value);
                m_sample_size_value = *m_sample_size;
            }
            return *this;
        }

        inline BlockIterator& operator--()
        {
            if (m_sample_size)
            {
                m_sample_size = reinterpret_cast<const std::uint32_t*>(
                    reinterpret_cast<const std::uint8_t*>(m_sample_size) - m_sample_size_stride - m_sample_size_value);
                m_sample_size_value = *m_sample_size;
            }

            if (m_data)
            {
                m_data = reinterpret_cast<const std::uint8_t*>(m_data) - m_data_stride - m_sample_size_value;
            }

            if (m_timestamp)
            {
                m_timestamp = reinterpret_cast<const std::uint64_t*>(
                    reinterpret_cast<const std::uint8_t*>(m_timestamp) - m_timestamp_stride - m_sample_size_value);
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

        std::uint64_t distanceTo(const BlockIterator& other) const
        {
            auto end_pos = reinterpret_cast<const std::uint8_t*>(other.m_data);
            auto start_pos = reinterpret_cast<const std::uint8_t*>(this->m_data);

            if(end_pos <= start_pos || this->m_data_stride != other.m_data_stride)
            {
                return 0;
            }

            return (end_pos - start_pos) / this->m_data_stride;
        }

    private:
        const void* m_data;
        std::size_t m_data_stride;
        const std::uint64_t* m_timestamp;
        std::size_t   m_timestamp_stride;
        std::uint64_t m_timestamp_value;

        const std::uint32_t* m_sample_size;
        std::size_t m_sample_size_stride;
        std::uint32_t m_sample_size_value;
    };
}
}
