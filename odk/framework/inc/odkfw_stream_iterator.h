// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkapi_data_set_descriptor_xml.h"
#include "odkfw_block_iterator.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace odk
{
namespace framework
{

    class StreamIterator;

    class IfIteratorUpdater
    {
    public:
        virtual void updateStreamIterator(StreamIterator* iterator) = 0;
    };

    class StreamIterator
    {
    public:
        StreamIterator();

        /// Start address of the sample
        const void* data() const;
        /// Timestamp of the sample
        std::uint64_t timestamp() const;

        template<class SampleFormat>
        SampleFormat value() const
        {
            return *static_cast<const SampleFormat*>(data());
        }

        bool valid() const;
        void addRange(const BlockIterator& begin, const BlockIterator& end);

        void clearRanges();

        void setDataRequester(IfIteratorUpdater* requester);

        StreamIterator& operator++();
        StreamIterator& operator--();
        bool operator==(const StreamIterator&) const;
        bool operator!=(const StreamIterator&) const;

    private:
        using BlockIteratorRange = std::pair<BlockIterator, BlockIterator>;
        std::vector<BlockIteratorRange> m_blocks_ranges;
        int m_block_index;
        BlockIterator m_current_iterator;
        IfIteratorUpdater* m_data_requester;
    };
}

}
