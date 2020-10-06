// Copyright DEWETRON GmbH 2017

#include "odkfw_stream_iterator.h"

namespace odk
{
namespace framework
{
    StreamIterator::StreamIterator()
        : m_block_index(-1)
        , m_data_requester(nullptr)
    {
    }

    const void* StreamIterator::data() const
    {
        return valid() ? m_current_iterator.data() : nullptr;
    }

    std::uint64_t StreamIterator::timestamp() const
    {
        return valid() ? m_current_iterator.timestamp() : 0;
    }

    StreamIterator& StreamIterator::operator++()
    {
        ++m_current_iterator;
        if (m_current_iterator == m_blocks_ranges[m_block_index].second)
        {
            ++m_block_index;
            if (m_block_index != m_blocks_ranges.size())
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
        }
        return *this;
    }

    StreamIterator& StreamIterator::operator--()
    {
        if (m_current_iterator == m_blocks_ranges[m_block_index].first)
        {
            --m_block_index;
            if (m_block_index < 0)
            {
                return *this;
            }
            else
            {
                m_current_iterator = m_blocks_ranges[m_block_index].second;
            }
        }
        --m_current_iterator;
        return *this;
    }

    bool StreamIterator::operator==(const StreamIterator& other) const
    {
        return m_block_index == other.m_block_index && m_current_iterator == other.m_current_iterator;
    }

    bool StreamIterator::operator!=(const StreamIterator& other) const
    {
        return !(*this == other);
    }

    bool StreamIterator::valid() const
    {
        return m_block_index >= 0;
    }

    void StreamIterator::addRange(const BlockIterator& begin, const BlockIterator& end)
    {
        m_blocks_ranges.emplace_back(begin, end);
        m_block_index = 0;
        m_current_iterator = m_blocks_ranges.front().first;
    }

    void StreamIterator::clearRanges()
    {
        m_blocks_ranges.clear();
        m_block_index = -1;
        m_current_iterator = BlockIterator();
    }

    void StreamIterator::setDataRequester(IfIteratorUpdater *requester)
    {
        m_data_requester = requester;
    }
}
}
