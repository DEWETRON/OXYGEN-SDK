// Copyright DEWETRON GmbH 2017

#include "odkfw_stream_iterator.h"

namespace odk
{
namespace framework
{
    StreamIterator::StreamIterator()
        : m_data_requester(nullptr)
        , m_valid(false)
        , m_signal_gaps(false)
        , m_skip_gaps(true)
    {
    }

    const StreamIterator::BlockIteratorRange* StreamIterator::selectBlock(std::uint64_t timestamp)
    {
        for(const auto& range_list : m_blocks_ranges)
        {
            for(const auto& range : range_list.second)
            {
                if(timestamp >= range.first.timestamp() && timestamp <= range.second.timestamp())
                {
                    return &range;
                }
            }
        }
        return nullptr;
    }

    const StreamIterator::BlockIteratorRange* StreamIterator::getNextBlock(std::uint64_t timestamp)
    {
        const BlockIteratorRange* next_range = nullptr;
        for(const auto& range_list : m_blocks_ranges)
        {
            for(auto it = range_list.second.begin(); it != range_list.second.end(); ++it)
            {
                if (it->first.timestamp() > timestamp)
                {
                    if(!next_range || it->first.timestamp() < next_range->first.timestamp())
                    {
                        next_range = &(*it);
                    }
                    break;
                }
            }
        }
        return next_range;
    }

    const StreamIterator::BlockIteratorRange* StreamIterator::getPreviousBlock(std::uint64_t timestamp)
    {
        const BlockIteratorRange* previous_range = nullptr;
        for(const auto& range_list : m_blocks_ranges)
        {
            for(auto it = range_list.second.rbegin(); it != range_list.second.rend(); ++it)
            {
                if (it->second.timestamp() < timestamp)
                {
                    if(!previous_range || it->second.timestamp() > previous_range->second.timestamp())
                    {
                        previous_range = &(*it);
                    }
                    break;
                }
            }
        }
        return previous_range;
    }

    void StreamIterator::getNextBlock()
    {
        const auto timestamp = m_current_iterator.timestamp() + 1;
        auto block = selectBlock(timestamp);

        if(!block)
        {
            block = getNextBlock(timestamp);;
        }

        if(block)
        {
            m_previous_border = block->first.timestamp();
            m_next_border = block->second.timestamp();

            m_current_iterator = block->first;
            if (m_current_iterator.timestamp() <= timestamp)
            {
                while (m_current_iterator.timestamp() <= timestamp)
                {
                    ++m_current_iterator;
                }
                --m_current_iterator;
            }

            auto next_block = getNextBlock(m_current_iterator.timestamp());
            if(next_block && next_block->first.timestamp() <= m_next_border)
            {
                m_next_border = next_block->first.timestamp() - 1;
            }
        }
        else if (m_data_requester)
        {
            m_data_requester->updateStreamIterator(this);
        }
        else
        {
            m_valid = false;
        }

        if(m_valid && m_current_iterator.data() == nullptr)
        {
            if(m_skip_gaps)
            {
                m_current_iterator = BlockIterator(m_next_border);
                getNextBlock();
            }
            if(m_signal_gaps)
            {
                throw std::runtime_error("data gap occured");
            }
        }
    }

    void StreamIterator::getPreviousBlock()
    {
        const auto timestamp = m_current_iterator.timestamp() - 1;
        auto block = selectBlock(timestamp);

        if(!block)
        {
            block = getPreviousBlock(timestamp);
        }

        if(block)
        {
            m_previous_border = block->first.timestamp();
            m_next_border = block->second.timestamp();

            m_current_iterator = block->second;
            if(m_current_iterator.timestamp() > timestamp)
            {
                while(m_current_iterator.timestamp() > timestamp)
                {
                    --m_current_iterator;
                }
            }

            auto previous_block = getPreviousBlock(m_current_iterator.timestamp());
            if (previous_block && previous_block->second.timestamp() >= m_next_border)
            {
                m_next_border = previous_block->second.timestamp() + 1;
            }

        }
        else
        {
            m_valid = false;
        }

        if(m_valid && m_current_iterator.data() == nullptr)
        {
            if(m_skip_gaps)
            {
                m_current_iterator = BlockIterator(m_previous_border);
                getPreviousBlock();
            }
            if(m_signal_gaps)
            {
                throw std::runtime_error("data gap occured");
            }
        }
    }

    void StreamIterator::addRange(const BlockIterator& begin, const BlockIterator& end, std::uint32_t priority)
    {
        m_blocks_ranges[priority].emplace(begin, end);
    }

    void StreamIterator::init(std::uint64_t tick)
    {
        m_valid = true;
        m_current_iterator = BlockIterator(tick - 1);
        getNextBlock();
    }

    void StreamIterator::setSignalGaps(bool enabled)
    {
        m_signal_gaps = enabled;
    }

    void StreamIterator::setSkipGaps(bool enabled)
    {
        m_skip_gaps = enabled;
    }

    void StreamIterator::clearRanges()
    {
        m_blocks_ranges.clear();
        m_next_border = 0;
        m_previous_border = 0;
        m_current_iterator = BlockIterator();
        m_valid = false;
    }

    void StreamIterator::setDataRequester(IfIteratorUpdater *requester)
    {
        m_data_requester = requester;
    }
}
}
