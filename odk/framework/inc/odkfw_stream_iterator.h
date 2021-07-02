// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkapi_data_set_descriptor_xml.h"
#include "odkapi_types.h"
#include "odkfw_block_iterator.h"

#include <cstddef>
#include <map>
#include <utility>
#include <set>

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
        inline const void* data() const
        {
            return valid() ? m_current_iterator.data() : nullptr;
        }

        /// Timestamp of the sample
        inline std::uint64_t timestamp() const
        {
            return valid() ? m_current_iterator.timestamp() : 0;
        }

        inline std::size_t size() const
        {
            return valid() ? m_current_iterator.size() : 0;
        }

        template<class SampleFormat>
        inline SampleFormat value() const
        {
            if (const void* data_ptr = data())
            {
                return *static_cast<const SampleFormat*>(data_ptr);
            }
            return std::numeric_limits<SampleFormat>::quiet_NaN();
        }

        inline bool valid() const
        {
            return m_block_index >= 0;
        }

        void addRange(const BlockIterator& begin, const BlockIterator& end);

        void clearRanges();

        void setDataRequester(IfIteratorUpdater* requester);

        inline StreamIterator& operator++()
        {
            ++m_current_iterator;
            if (m_current_iterator == m_blocks_ranges[m_block_index].second)
            {
                getNextBlock();
            }
            return *this;
        }

        inline StreamIterator& operator--()
        {
            if (m_current_iterator == m_blocks_ranges[m_block_index].first)
            {
                getPreviousBlock();
            }
            --m_current_iterator;
            return *this;
        }


        inline bool operator==(const StreamIterator& other) const
        {
            return m_current_iterator == other.m_current_iterator;
        };

        inline bool operator!=(const StreamIterator& other) const
        {
            return !(*this == other);
        }

        void setSignalGaps(bool enabled);
        void setSkipGaps(bool enabled);

        std::uint64_t getTotalSampleCount() const;

        using BlockIteratorRange = std::pair<BlockIterator, BlockIterator>;

    private:
        void getNextBlock();
        void getPreviousBlock();

    private:
        std::vector<BlockIteratorRange> m_blocks_ranges;
        int m_block_index;
        BlockIterator m_current_iterator;
        IfIteratorUpdater* m_data_requester;
        bool m_signal_gaps;
        bool m_skip_gaps;
    };
}

}
