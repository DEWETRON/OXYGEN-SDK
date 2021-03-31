// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkapi_block_descriptor_xml_fwd.h"
#include "odkapi_types.h"
#include "odkbase_if_value.h"

#include <map>
#include <string>
#include <vector>

namespace odk
{
    class BlockChannelDescriptor
    {
    public:
        BlockChannelDescriptor();

        std::uint32_t m_offset;             ///< bit offset of first sample, stride from scan descriptor
        std::uint64_t m_channel_id;

        // time info in ticks
        std::uint64_t m_timestamp;          ///< first timestamp covered by block
        std::uint64_t m_duration;           ///< duration covered by block
        std::uint64_t m_first_sample_index; ///< sample index of the first sample in the block
        std::uint64_t m_count;              ///< number of samples in the block
    };

    class BlockDescriptor
    {
    public:
        BlockDescriptor();
        BlockDescriptor(const BlockDescriptor& bd) = default;
        BlockDescriptor(BlockDescriptor&&);

        BlockDescriptor& operator=(const BlockDescriptor& bd) = default;

        bool parse(const char* xml_string);

        std::string generate() const;

        std::uint64_t m_stream_id;
        std::uint64_t m_data_size; ///< size of the complete data block in bytes
        std::vector<BlockChannelDescriptor> m_block_channels;
    };

    class DataRegion
    {
    public:
        DataRegion() = delete;
        DataRegion(std::uint64_t channel_id, const Interval<std::uint64_t>& region);

        std::uint64_t m_channel_id;
        Interval<std::uint64_t> m_region;
    };

    class BlockListDescriptor
    {
    public:

        BlockListDescriptor();

        bool parse(const char* xml_string);
        std::string generate() const;

        std::uint32_t m_block_count;

        std::vector<Interval<double>> m_windows;
        std::vector<DataRegion> m_invalid_regions;
    };

    class DataRegions
    {
    public:
        DataRegions() = default;

        bool parse(const char* xml_string);
        std::string generate() const;

        std::vector<DataRegion> m_data_regions;
    };

}
