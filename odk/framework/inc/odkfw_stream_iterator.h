// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkapi_data_set_descriptor.h"
#include "odkfw_block_iterator.h"

#include <cstddef>
#include <utility>
#include <vector>

namespace odk
{
namespace framework
{
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

        StreamIterator& operator++();
        StreamIterator& operator--();
        bool operator==(const StreamIterator&) const;
        bool operator!=(const StreamIterator&) const;

    private:
        using BlockIteratorRange = std::pair<BlockIterator, BlockIterator>;
        std::vector<BlockIteratorRange> m_blocks_ranges;
        int m_block_index;
        BlockIterator m_current_iterator;
    };
}

}
