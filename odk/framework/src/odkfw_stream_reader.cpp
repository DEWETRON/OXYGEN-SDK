// Copyright DEWETRON GmbH 2017
#include "odkfw_stream_reader.h"
#include "odkapi_data_set_descriptor_xml.h"
#include "odkuni_assert.h"

#include <stdexcept>

namespace odk
{
namespace framework
{
    StreamReader::StreamReader()
        : m_stream_descriptor()
    {
    }

    StreamReader::StreamReader(const StreamDescriptor& stream_descriptor)
        : m_stream_descriptor(stream_descriptor)
    {
    }

    void StreamReader::setStreamDescriptor(const StreamDescriptor& stream_descriptor)
    {
        m_stream_descriptor = stream_descriptor;
    }

    void StreamReader::addDataBlock(const BlockDescriptor& block_descriptor, const void* data)
    {
        m_blocks.emplace(block_descriptor.m_stream_id, std::make_tuple(block_descriptor, data));
    }

    void StreamReader::addDataBlock(BlockDescriptor&& block_descriptor, const void* data)
    {
        m_blocks.emplace(block_descriptor.m_stream_id, std::make_tuple(std::move(block_descriptor), data));
    }

    const ChannelDescriptor* StreamReader::getChannelDescriptor(const std::uint64_t channel_id) const
    {
        auto channel_descriptor = std::find_if(
            m_stream_descriptor.m_channel_descriptors.begin(), m_stream_descriptor.m_channel_descriptors.end(),
            [channel_id](const ChannelDescriptor& desc)
            {
                return desc.m_channel_id == channel_id;
            });

        if (channel_descriptor != m_stream_descriptor.m_channel_descriptors.end())
        {
            return &(*channel_descriptor);
        }
        else
        {
            return nullptr;
        }
    }

    bool StreamReader::hasChannel(const std::uint64_t channel_id) const
    {
        return getChannelDescriptor(channel_id) != nullptr;
    }

    StreamIterator StreamReader::createChannelIterator(std::uint64_t channel_id) const
    {
        std::uint64_t dummy_num_of_samples = 0;
        return createChannelIterator(channel_id, dummy_num_of_samples);
    }
    StreamIterator StreamReader::createChannelIterator(std::uint64_t channel_id, std::uint64_t& sample_count) const
    {
        sample_count = 0;
        auto channel_descriptor = getChannelDescriptor(channel_id);
        if (!channel_descriptor)
        {
            throw std::runtime_error("Invalid channel ID");
        }
        StreamIterator iterator;
        auto blocks_begin = m_blocks.lower_bound(m_stream_descriptor.m_stream_id);
        auto blocks_end = m_blocks.upper_bound(m_stream_descriptor.m_stream_id);
        for (auto it_block = blocks_begin; it_block != blocks_end; ++it_block)
        {
            const BlockDescriptor& block_descriptor = std::get<0>(it_block->second);
            const void* block_data = std::get<1>(it_block->second);

            for (const auto& bcd : block_descriptor.m_block_channels)
            {
                if (bcd.m_channel_id == channel_id)
                {
                    ODK_ASSERT_EQUAL(bcd.m_offset % 8, 0);

                    auto offset_bytes = bcd.m_offset / 8;

                    const std::uint8_t* channel_data = reinterpret_cast<const std::uint8_t*>(block_data) + offset_bytes;

                    ODK_ASSERT_EQUAL(channel_descriptor->m_stride % 8, 0);

                    const std::size_t data_stride_bytes = channel_descriptor->m_stride / 8;
                    if (channel_descriptor->m_timestamp_position)
                    {
                        ODK_ASSERT_EQUAL(*channel_descriptor->m_timestamp_position % 8, 0);

                        const auto timestamp_pos_bytes = *channel_descriptor->m_timestamp_position / 8;

                        // Explicit Timestamp field
                        BlockIterator it_block_begin(channel_data, data_stride_bytes, reinterpret_cast<const uint64_t*>(channel_data + timestamp_pos_bytes), data_stride_bytes);
                        BlockIterator it_block_end(channel_data + data_stride_bytes * bcd.m_count, data_stride_bytes, reinterpret_cast<const std::uint64_t*>(channel_data + data_stride_bytes * bcd.m_count + timestamp_pos_bytes), data_stride_bytes);
                        iterator.addRange(it_block_begin, it_block_end);
                    }
                    else
                    {
                        // Implicit timestamps, incremented every sample
                        BlockIterator it_block_begin(channel_data, data_stride_bytes, bcd.m_first_sample_index);
                        BlockIterator it_block_end(channel_data + data_stride_bytes * bcd.m_count, data_stride_bytes, bcd.m_first_sample_index + bcd.m_count);
                        iterator.addRange(it_block_begin, it_block_end);
                    }
                    sample_count += bcd.m_count;
                }
            }
        }

        return iterator;
    }
}
}
