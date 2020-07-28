// Copyright DEWETRON GmbH 2019

#pragma once

#include "odkapi_pugixml_fwd.h"
#include "odkapi_types.h"

#include <string>

namespace odk
{

    struct ChannelDataformat : equality_comparable<ChannelDataformat>
    {
        enum class SampleFormat
        {
            INVALID,
            NONE,

            SINT8,
            SINT16,
            SINT24,
            SINT32,
            SINT64,
            UINT8,
            UINT16,
            UINT32,
            UINT64,

            FLOAT,
            DOUBLE,

            COMPLEX_FLOAT,
            COMPLEX_DOUBLE,

            VIDEO_RAW_FRAME,

            UTF8_STRING,

            CAN_MESSAGE,
            FLEXRAY_MESSAGE,
            RES_NUM_TYPES = FLEXRAY_MESSAGE
        };

        enum class SampleOccurrence
        {
            SYNC,
            ASYNC,
            SINGLE_VALUE,
            NEVER,
            INVALID
        };

        enum class SampleReducedFormat
        {
            UNKNOWN,
            R_R_R,
            R_SF_SF
        };

        enum class SampleValueType
        {
            SAMPLE_VALUE_INVALID,
            SAMPLE_VALUE_SCALAR,
            SAMPLE_VALUE_CAN_MESSAGE,
            SAMPLE_VALUE_VECTOR,
            SAMPLE_VALUE_COMPLEX_VECTOR,
            SAMPLE_VALUE_VIDEO,
            SAMPLE_VALUE_STRING,
            SAMPLE_VALUE_FLEXRAY_MESSAGE
        };

        SampleFormat m_sample_format = SampleFormat::INVALID;
        SampleValueType m_sample_value_type = SampleValueType::SAMPLE_VALUE_INVALID;
        std::uint32_t m_sample_dimension = 0;
        SampleOccurrence m_sample_occurrence = SampleOccurrence::INVALID;
        SampleReducedFormat m_sample_reduced_format = SampleReducedFormat::UNKNOWN;

        bool store(pugi::xml_node parent_node) const;
        bool extract(pugi::xml_node parent_node);

        std::string generate() const;
        bool parse(const char* xml_string);
        bool parse(pugi::xml_node data_format);

        bool operator==(const ChannelDataformat& other) const
        {
            return m_sample_format == other.m_sample_format
                && m_sample_dimension == other.m_sample_dimension
                && m_sample_occurrence == other.m_sample_occurrence
                && m_sample_reduced_format == other.m_sample_reduced_format
                && m_sample_value_type == other.m_sample_value_type;
            ;
        }

        static std::string getSampleFormatString(SampleFormat f);
    };

    struct ChannelDataformatTelegram : equality_comparable<ChannelDataformatTelegram>
    {
        bool store(pugi::xml_node parent_node) const;
        bool extract(pugi::xml_node parent_node);

        std::string generate() const;
        bool parse(const char* xml_string);
        bool parse(pugi::xml_node channel_node);

        std::uint64_t channel_id;
        ChannelDataformat data_format;

        bool operator==(const ChannelDataformatTelegram& other) const
        {
            return channel_id == other.channel_id
                && data_format == other.data_format;
        }
    };
}
