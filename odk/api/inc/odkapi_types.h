// Copyright DEWETRON GmbH 2017
#pragma once

#include <cstdint>
#include <string>

namespace odk
{

    typedef std::uint64_t ChannelID;

    template <class T>
    class equality_comparable
    {
    public:
        bool operator!=(const T& other) const
        {
            return !static_cast<bool>(*static_cast<const T*>(this) == other);
        }
    };

    template <class T>
    class Interval
    {
    public:
        Interval()
            : m_begin(0)
            , m_end(0)
        {}
        explicit Interval(T begin, T end)
            : m_begin(begin)
            , m_end(end)
        {}

        bool operator< (const Interval& rhs) const
        {
            if (m_begin == rhs.m_begin)
            {
                return m_end < rhs.m_end;
            }
            return m_begin < rhs.m_begin;
        }
        bool operator==(const Interval& rhs) const
        {
            return m_begin == rhs.m_begin && m_end == rhs.m_end;
        }

        T m_begin;
        T m_end;
    };

    enum class DataSetType
    {
        RAW = 0,
        SCALED = 1
    };

    enum class DataSetMode
    {
        NORMAL = 0,
        SKIP,
        AVERAGE,
        REDUCED,
        INVALID = -1
    };

    enum class StreamPolicy
    {
        EXACT = 0,
        RELAXED
    };

    enum class StreamType
    {
        PUSH = 0,
        PULL
    };

    std::string dataSetModeToString(const DataSetMode& mode);
    DataSetMode stringToDataSetMode(const std::string& string);

    std::string dataSetTypeToString(const DataSetType& type);
    DataSetType stringToDataSetType(const std::string& string);

    std::string streamPolicyToString(const StreamPolicy& policy);
    StreamPolicy stringToStreamPolicy(const std::string& string);

    std::string streamTypeToString(const StreamType& type);
    StreamType stringToStreamType(const std::string& string);
}

