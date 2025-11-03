// Copyright DEWETRON GmbH 2017
#include "odkfw_stream_reader.h"
#include "odkapi_data_set_descriptor_xml.h"
#include "odkuni_assert.h"

#include <algorithm>
#include <stdexcept>

namespace odk
{
namespace framework
{
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

    void StreamReader::addDataRegion(const odk::DataRegion& region)
    {
        m_data_regions[region.m_channel_id].emplace(region.m_region);
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
        return createChannelIterator(channel_id, odk::Interval<std::uint64_t>(0, std::numeric_limits<std::uint64_t>::max()));
    }

    StreamIterator StreamReader::createChannelIterator(std::uint64_t channel_id, const odk::Interval<std::uint64_t>& interval) const
    {
        StreamIterator iterator;
        updateStreamIterator(channel_id, iterator, interval);
        return iterator;
    }

    void StreamReader::updateStreamIterator(std::uint64_t channel_id, StreamIterator& iterator, const odk::Interval<std::uint64_t>& interval) const
    {
        std::uint64_t sample_count = 0;
        iterator.clearRanges();

        auto channel_descriptor = getChannelDescriptor(channel_id);
        if (!channel_descriptor)
        {
            throw std::runtime_error("Invalid channel ID");
        }

        std::multimap<std::uint64_t, BlockDescriptorData>::const_iterator blocks_begin;
        std::multimap<std::uint64_t, BlockDescriptorData>::const_iterator blocks_end;
        std::tie(blocks_begin, blocks_end) = m_blocks.equal_range(m_stream_descriptor.m_stream_id);

        for (auto it_block = blocks_begin; it_block != blocks_end; ++it_block)
        {
            const BlockDescriptor& block_descriptor = std::get<0>(it_block->second);
            const void* block_data = std::get<1>(it_block->second);

            for (const auto& bcd : block_descriptor.m_block_channels)
            {
                if (bcd.m_channel_id == channel_id && bcd.m_count > 0)
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

                        if (channel_descriptor->m_sample_size_position)
                        {
                            ODK_ASSERT_EQUAL(*channel_descriptor->m_sample_size_position % 8, 0);

                            const auto sample_size_bytes = *channel_descriptor->m_sample_size_position / 8;
                            // Explicit Timestamp field and sample size field
                            BlockIterator it_block_begin(channel_data, data_stride_bytes, reinterpret_cast<const uint64_t*>(channel_data + timestamp_pos_bytes), 
                                data_stride_bytes, reinterpret_cast<const uint32_t*>(channel_data + sample_size_bytes), data_stride_bytes);

                            if (block_descriptor.m_block_channels.size() == 1)
                            {
                                const std::uint8_t* data_end = channel_data + block_descriptor.m_data_size;

                                const std::uint64_t* ts_end = reinterpret_cast<const uint64_t*>(data_end + timestamp_pos_bytes);
                                const std::uint32_t* size_end = reinterpret_cast<const uint32_t*>(data_end + sample_size_bytes);

                                BlockIterator it_block_end(data_end, data_stride_bytes, ts_end, data_stride_bytes, size_end, data_stride_bytes);
                                iterator.addRange(it_block_begin, it_block_end);
                            }
                            else
                            {
                                BlockIterator it_block_end = it_block_begin;

                                for (std::uint64_t i = 0; i < bcd.m_count; i++)
                                {
                                    ++it_block_end;
                                }

                                iterator.addRange(it_block_begin, it_block_end);
                            }
                        }
                        else
                        {
                            // Explicit Timestamp field
                            BlockIterator it_block_begin(channel_data, data_stride_bytes, reinterpret_cast<const uint64_t*>(channel_data + timestamp_pos_bytes), data_stride_bytes);
                            BlockIterator it_block_end(channel_data + data_stride_bytes * bcd.m_count, data_stride_bytes, reinterpret_cast<const std::uint64_t*>(channel_data + data_stride_bytes * bcd.m_count + timestamp_pos_bytes), data_stride_bytes);
                            iterator.addRange(it_block_begin, it_block_end);
                        }
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

        auto regions = m_data_regions.find(channel_id);

        std::uint64_t invalid_region_start = interval.m_begin;
        std::uint64_t invalid_region_end = interval.m_end;

        if(regions != m_data_regions.end())
        {
            for(const auto& valid_region : regions->second)
            {
                invalid_region_end = valid_region.m_begin;

                if(invalid_region_end > invalid_region_start)
                {
                    BlockIterator it_block_begin(invalid_region_start);
                    BlockIterator it_block_end(invalid_region_end);
                    iterator.addRange(it_block_begin, it_block_end);
                }

                invalid_region_start = valid_region.m_end;
            }
        }

        invalid_region_end = interval.m_end;

        if (invalid_region_end > invalid_region_start)
        {
            BlockIterator it_block_begin(invalid_region_start);
            BlockIterator it_block_end(invalid_region_end);
            iterator.addRange(it_block_begin, it_block_end);
        }

        ODK_UNUSED(sample_count);
        //ODK_ASSERT_EQUAL(iterator.getTotalSampleCount(), sample_count);
    }

    void StreamReader::clearBlocks()
    {
        m_blocks.clear();
        m_data_regions.clear();
    }

}
}
