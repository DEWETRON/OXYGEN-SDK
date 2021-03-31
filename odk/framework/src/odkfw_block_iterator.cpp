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
        , m_timestamp_stride(0)
        , m_timestamp_value(initial_timestamp)
    {
    }

    BlockIterator::BlockIterator(const void* data, std::size_t data_stride,
                                   const std::uint64_t* timestamp, std::size_t timestamp_stride)
        : m_data(data)
        , m_data_stride(data_stride)
        , m_timestamp(timestamp)
        , m_timestamp_stride(timestamp_stride)
        , m_timestamp_value(*timestamp)
    {
    }

    BlockIterator::BlockIterator(const std::uint64_t timestamp)
        : m_data(nullptr)
        , m_data_stride(0)
        , m_timestamp(nullptr)
        , m_timestamp_stride(0)
        , m_timestamp_value(timestamp)
    {
    }
}
}
