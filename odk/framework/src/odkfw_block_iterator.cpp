// Copyright DEWETRON GmbH 2017

#include "odkfw_block_iterator.h"

namespace odk
{
namespace framework
{
    BlockIterator::BlockIterator() noexcept
        : m_data(nullptr)
        , m_data_stride(0)
        , m_timestamp(nullptr)
        , m_timestamp_stride(0)
        , m_timestamp_value(0u)
        , m_sample_size(nullptr)
        , m_sample_size_stride(0)
        , m_sample_size_value(0)
    {
    }

    BlockIterator::BlockIterator(const void* data, std::size_t data_stride, std::uint64_t initial_timestamp) noexcept
        : m_data(data)
        , m_data_stride(data_stride)
        , m_timestamp(nullptr)
        , m_timestamp_stride(0)
        , m_timestamp_value(initial_timestamp)
        , m_sample_size(nullptr)
        , m_sample_size_stride(0)
        , m_sample_size_value(0)
    {
    }

    BlockIterator::BlockIterator(const void* data, std::size_t data_stride,
                                 const std::uint64_t* timestamp, std::size_t timestamp_stride) noexcept
        : m_data(data)
        , m_data_stride(data_stride)
        , m_timestamp(timestamp)
        , m_timestamp_stride(timestamp_stride)
        , m_timestamp_value(*timestamp)
        , m_sample_size(nullptr)
        , m_sample_size_stride(0)
        , m_sample_size_value(0)
    {
    }

    BlockIterator::BlockIterator(const void* data, std::size_t data_stride,
                                 const std::uint64_t* timestamp, std::size_t timestamp_stride,
                                 const std::uint32_t* sample_size, std::size_t sample_size_stride) noexcept
        : m_data(data)
        , m_data_stride(data_stride)
        , m_timestamp(timestamp)
        , m_timestamp_stride(timestamp_stride)
        , m_timestamp_value(*timestamp)
        , m_sample_size(sample_size)
        , m_sample_size_stride(sample_size_stride)
        , m_sample_size_value(*sample_size)
    {
    }

    BlockIterator::BlockIterator(const std::uint64_t timestamp) noexcept
        : m_data(nullptr)
        , m_data_stride(0)
        , m_timestamp(nullptr)
        , m_timestamp_stride(0)
        , m_timestamp_value(timestamp)
        , m_sample_size(nullptr)
        , m_sample_size_stride(0)
        , m_sample_size_value(0)
    {
    }

    BlockIterator& BlockIterator::operator++()
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

    BlockIterator& BlockIterator::operator--()
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

    std::uint64_t BlockIterator::distanceTo(const BlockIterator& other) const noexcept
    {
        auto end_pos = reinterpret_cast<const std::uint8_t*>(other.m_data);
        auto start_pos = reinterpret_cast<const std::uint8_t*>(this->m_data);

        if (!end_pos || !start_pos || end_pos < start_pos || this->m_data_stride != other.m_data_stride)
        {
            return 0;
        }

        if (m_sample_size)
        {
            auto copy = *this;

            std::uint64_t count = 0;
            while (copy != other)
            {
                ++copy;
                ++count;
            }
            return count;
        }
        else
        {
            return (end_pos - start_pos) / this->m_data_stride;
        }
    }
}
}
