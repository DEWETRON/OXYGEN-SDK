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
        , m_sample_size(nullptr)
        , m_sample_size_stride(0)
        , m_sample_size_value(0)
    {
    }

    BlockIterator::BlockIterator(const void* data, std::size_t data_stride, std::uint64_t initial_timestamp)
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
                                 const std::uint64_t* timestamp, std::size_t timestamp_stride)
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
                                 const std::uint32_t* sample_size, std::size_t sample_size_stride)
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

    BlockIterator::BlockIterator(const std::uint64_t timestamp)
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
}
}
