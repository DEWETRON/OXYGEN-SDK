// Copyright DEWETRON GmbH 2017

#include "odkfw_stream_iterator.h"

#include "odkapi_block_descriptor_xml.h"
#include "odkapi_utils.h"

namespace odk
{
namespace framework
{
    StreamIterator::StreamIterator() noexcept
        : m_block_index(-1)
        , m_data_requester(nullptr)
        , m_signal_gaps(false)
        , m_skip_gaps(true)
    {
    }

    void StreamIterator::getNextBlock()
    {
        bool skip = true;

        while(skip)
        {
            ++m_block_index;

            if (m_block_index != static_cast<int>(m_blocks_ranges.size()))
            {
                m_current_iterator = m_blocks_ranges[m_block_index].first;
            }
            else if(m_data_requester)
            {
                m_data_requester->updateStreamIterator(this);
            }
            else
            {
                m_block_index = -1;
            }

            skip = valid() && m_skip_gaps && data() == nullptr;
        }
    }

    void StreamIterator::getPreviousBlock()
    {
        bool skip = true;

        while(skip)
        {
            --m_block_index;
            if (m_block_index >= 0)
            {
                m_current_iterator = m_blocks_ranges[m_block_index].second;
            }
            skip = valid() && m_skip_gaps && data() == nullptr;
        }
    }

    void StreamIterator::addRange(const BlockIterator& begin, const BlockIterator& end)
    {
        auto predecessor = m_blocks_ranges.rbegin();
        while(predecessor != m_blocks_ranges.rend())
        {
            if(predecessor->first.timestamp() < begin.timestamp())
            {
                break;
            }
            ++predecessor;
        }
        m_blocks_ranges.emplace(predecessor.base(), begin, end);

        m_block_index = 0;
        m_current_iterator = m_blocks_ranges.front().first;
        if (m_skip_gaps && data() == nullptr)
        {
            getNextBlock();
        }
    }

    void StreamIterator::clearRanges() noexcept
    {
        m_blocks_ranges.clear();
        m_block_index = -1;
        m_current_iterator = {};
    }

    void StreamIterator::setSignalGaps(bool enabled) noexcept
    {
        m_signal_gaps = enabled;
    }

    void StreamIterator::setSkipGaps(bool enabled)
    {
        m_skip_gaps = enabled;

        if(!m_blocks_ranges.empty())
        {
            m_block_index = 0;
            m_current_iterator = m_blocks_ranges.front().first;
            if (m_skip_gaps && data() == nullptr)
            {
                getNextBlock();
            }
        }
    }

    bool StreamIterator::isInGap(double timestamp) const
    {
        auto ts = odk::convertTimeToTickAtOrAfter(timestamp, m_timebase);

        if (m_blocks_ranges.empty()) return true;

        for (auto rng = m_blocks_ranges.begin();
            rng != m_blocks_ranges.end(); rng++)
        {
            bool interval_pause = rng->first.data() == nullptr;

            if(interval_pause)
            {
                uint64_t interval_begin = rng->first.timestamp();
                uint64_t interval_end = rng->second.timestamp();
                if ((ts >= interval_begin) && (ts < interval_end))
                {
                    return true;
                }
            }
        }

        return false;
    }

    void StreamIterator::setDataRequester(IfIteratorUpdater *requester) noexcept
    {
        m_data_requester = requester;
    }

    std::uint64_t StreamIterator::getTotalSampleCount() const noexcept
    {
        std::uint64_t sample_count = 0;
        for(const auto& block_range : m_blocks_ranges)
        {
            sample_count += block_range.first.distanceTo(block_range.second);
        }
        return sample_count;
    }

    std::vector<DataRegion> StreamIterator::getDataRegions(double start, double end) const noexcept
    {
        if (m_data_requester)
        {
            return m_data_requester->getDataRegions(start, end);
        }
        return {};
    }

    void StreamIterator::setTimebase(const odk::Timebase& timebase) noexcept
    {
        m_timebase = timebase;
    }

    const odk::Timebase& StreamIterator::getTimebase() const noexcept
    {
        return m_timebase;
    }

    double StreamIterator::getTime() noexcept
    {
        return odk::convertTickToTime(timestamp(), m_timebase);
    }

}
}
