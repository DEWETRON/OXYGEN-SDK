// Copyright DEWETRON GmbH 2017

#include "odkfw_block_iterator.h"

namespace odk
{
namespace framework
{
    BlockIterator::BlockIterator()
        : m_data(nullptr)
        , m_data_stride(0)
        , m_timestamp(nullptr)
        , m_timestamp_value(0u)
    {
    }

    BlockIterator::BlockIterator(const void* data, std::size_t data_stride, std::uint64_t initial_timestamp)
        : m_data(data)
        , m_data_stride(data_stride)
        , m_timestamp(nullptr)
        , m_timestamp_value(initial_timestamp)
    {
    }

    BlockIterator::BlockIterator(const void* data, std::size_t data_stride,
                                   const std::uint64_t* timestamp, std::size_t timestamp_stride)
        : m_data(data)
        , m_data_stride(data_stride)
        , m_timestamp(timestamp)
        , m_timestamp_stride(timestamp_stride)
    {
    }

    BlockIterator& BlockIterator::operator++()
    {
        if (m_data)
        {
            m_data = reinterpret_cast<const std::uint8_t*>(m_data) + m_data_stride;
            if (m_timestamp)
            {
                m_timestamp = reinterpret_cast<const std::uint64_t*>(
                    reinterpret_cast<const std::uint8_t*>(m_timestamp) + m_timestamp_stride);
            }
            else
            {
                ++m_timestamp_value;
            }
        }
        return *this;
    }

    BlockIterator& BlockIterator::operator--()
    {
        if (m_data)
        {
            m_data = reinterpret_cast<const std::uint8_t*>(m_data) - m_data_stride;
            if (m_timestamp)
            {
                m_timestamp = reinterpret_cast<const std::uint64_t*>(
                    reinterpret_cast<const std::uint8_t*>(m_timestamp) - m_timestamp_stride);
            }
            else
            {
                --m_timestamp_value;
            }
        }
        return *this;
    }

    bool BlockIterator::operator==(const BlockIterator& other) const
    {
        // sufficient equality if the iterator points to the same data
        return m_data == other.m_data;
    }

    bool BlockIterator::operator!=(const BlockIterator& other) const
    {
        return !(*this == other);
    }

}
}
