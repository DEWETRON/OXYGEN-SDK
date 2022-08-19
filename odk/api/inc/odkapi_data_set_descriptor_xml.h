// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkapi_data_set_descriptor_xml_fwd.h"
#include "odkbase_if_value.h"
#include "odkuni_defines.h"

#include <boost/optional.hpp>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace odk
{

    enum class SampleType
    {
        DOUBLE = 0,
        SIGNED16BIT,
        SIGNED24BIT,
        SINGLEBIT,
        COMPLEX,
        BLOB,
        REDUCED,
        RAW
    };

    enum class ScalingType
    {
        LINEAR = 0,
    };

    class Scaling
    {
    public:
        Scaling() noexcept;
        Scaling(ScalingType type) noexcept;
        Scaling(ScalingType type, double factor, double offset) noexcept;

        ScalingType m_type;

        double m_factor;
        double m_offset;

        ODK_NODISCARD bool operator==(const Scaling& other) const noexcept;
        ODK_NODISCARD bool operator!=(const Scaling& other) const noexcept;
    };

    enum class TimeStampFormat
    {
        CLASSIC = 0
    };

    class ChannelDescriptor
    {
    public:

        ChannelDescriptor() noexcept;

        /// Unique channel identifier
        std::uint64_t m_channel_id;
        /// Stride information in bits
        std::uint32_t m_stride;
        /// Scaling information
        std::vector<Scaling> m_scaling;
        /// Timestamp format
        TimeStampFormat m_ts_format;
        /// Size of one sample in bits
        std::uint32_t m_size;
        /// Datatype of the sample
        SampleType m_type;
        /// Number of dimensions in one sample
        std::uint32_t m_dimension;

        /// Position of timestamp value relative to sample position
        boost::optional<std::int32_t> m_timestamp_position;

        boost::optional<std::int32_t> m_sample_size_position;

        ODK_NODISCARD bool operator==(const ChannelDescriptor& other) const;
        ODK_NODISCARD bool operator!=(const ChannelDescriptor& other) const;
    };

    class StreamDescriptor
    {
    public:
        StreamDescriptor() noexcept;

        std::uint64_t m_stream_id;
        std::vector<ChannelDescriptor> m_channel_descriptors;

        ODK_NODISCARD bool operator==(const StreamDescriptor& other) const;
        ODK_NODISCARD bool operator!=(const StreamDescriptor& other) const;
    };

    /**
     * class DataSetDescriptor
     */
    class DataSetDescriptor
    {
    public:
        DataSetDescriptor() noexcept;

        bool parse(const std::string_view& xml_string);

        ODK_NODISCARD std::string generate() const;

        std::uint64_t m_id;
        std::vector<StreamDescriptor> m_stream_descriptors;

        ODK_NODISCARD bool operator==(const DataSetDescriptor& other) const;
        ODK_NODISCARD bool operator!=(const DataSetDescriptor& other) const;
    };
}
