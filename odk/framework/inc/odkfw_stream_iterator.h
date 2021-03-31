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
            return m_valid ? m_current_iterator.data() : nullptr;
        }

        /// Timestamp of the sample
        inline std::uint64_t timestamp() const
        {
            return m_valid ? m_current_iterator.timestamp() : 0;
        }

        template<class SampleFormat>
        inline SampleFormat value() const
        {
            if(data())
            {
                return *static_cast<const SampleFormat*>(data());
            }
            return std::numeric_limits<SampleFormat>::quiet_NaN();
        }

        inline bool valid() const
        {
            return m_valid;
        }

        void addRange(const BlockIterator& begin, const BlockIterator& end, std::uint32_t priority = 0);

        void clearRanges();

        void setDataRequester(IfIteratorUpdater* requester);

        inline StreamIterator& operator++()
        {
            if (m_current_iterator.timestamp() >= m_next_border)
            {
                getNextBlock();
            }
            else
            {
                ++m_current_iterator;
            }
            return *this;
        }

        inline StreamIterator& operator--()
        {
            if (m_current_iterator.timestamp() <= m_previous_border)
            {
                getPreviousBlock();
            }
            else
            {
                --m_current_iterator;
            }
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

        void init(std::uint64_t tick = 0);

        void setSignalGaps(bool enabled);
        void setSkipGaps(bool enabled);

        using BlockIteratorRange = std::pair<BlockIterator, BlockIterator>;

    private:
        void getNextBlock();
        void getPreviousBlock();

        const BlockIteratorRange* selectBlock(std::uint64_t timestamp);
        const BlockIteratorRange* getNextBlock(std::uint64_t timestamp);
        const BlockIteratorRange* getPreviousBlock(std::uint64_t timestamp);

    private:
        std::map<uint32_t, std::set<BlockIteratorRange>, std::greater<uint32_t>> m_blocks_ranges;
        std::uint64_t m_next_border;
        std::uint64_t m_previous_border;
        BlockIterator m_current_iterator;
        IfIteratorUpdater* m_data_requester;
        bool m_valid;
        bool m_signal_gaps;
        bool m_skip_gaps;
    };
}

}
