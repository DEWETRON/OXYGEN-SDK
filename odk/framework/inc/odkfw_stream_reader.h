// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkapi_data_set_descriptor_xml.h"
#include "odkapi_block_descriptor_xml.h"
#include "odkfw_stream_iterator.h"
#include "odkuni_defines.h"

#include <cstddef>
#include <map>
#include <set>

namespace odk
{
namespace framework
{
    class StreamReader
    {
    public:
        /**
         * Creates a stream reader with no stream descriptor
         */
        StreamReader() = default;

        /**
         * Creates a stream reader with a stream descriptor
         */
        StreamReader(const StreamDescriptor& stream_descriptor);

        /**
         * Sets a new stream descriptor
         */
        void setStreamDescriptor(const StreamDescriptor& stream_descriptor);

        /**
         * Adds a block of data
         * blocks of incompatible streams will be ignored
         */
        void addDataBlock(const BlockDescriptor& block_descriptor, const void* data);
        void addDataBlock(BlockDescriptor&& block_descriptor, const void* data);

        void addDataRegion(const odk::DataRegion& region);

        /**
         * Returns an iterator for a specific channel that iterates across all compatible blocks
         */
        ODK_NODISCARD StreamIterator createChannelIterator(std::uint64_t channel_id) const;

        /**
         * Returns an iterator for a specific channel that iterates across all compatible blocks
         */
        ODK_NODISCARD StreamIterator createChannelIterator(std::uint64_t channel_id, const odk::Interval<std::uint64_t>& interval) const;

        void updateStreamIterator(std::uint64_t channel_id, StreamIterator& iterator, const odk::Interval<std::uint64_t>& interval) const;
        /**
         * Returns true if the channel is included in the stream descriptor
         */
        ODK_NODISCARD bool hasChannel(std::uint64_t channel_id) const;

        void clearBlocks();

    private:
        const ChannelDescriptor* getChannelDescriptor(std::uint64_t channel_id) const;

        using BlockDescriptorData = std::tuple<BlockDescriptor, const void*>;
        StreamDescriptor m_stream_descriptor;
        std::multimap<std::uint64_t, BlockDescriptorData> m_blocks;
        std::map<std::uint64_t, std::set<odk::Interval<std::uint64_t>>> m_data_regions;
    };
}
}
