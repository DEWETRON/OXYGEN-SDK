// Copyright DEWETRON GmbH 2019

#include "odkapi_channel_dataformat_xml.h"
#include "odkapi_utils.h"
#include "assert_bimap_size.h"
#include "odkuni_bimap.h"
#include "odkuni_xpugixml.h"


namespace odk
{
    typedef odk::SimpleBiMap<ChannelDataformat::SampleOccurrence, std::string> SampleOccurrenceStringMap;
    typedef odk::SimpleBiMap<ChannelDataformat::SampleFormat, std::string> SampleFormatStringMap;
    typedef odk::SimpleBiMap<ChannelDataformat::SampleReducedFormat, std::string> SampleReducedFormatStringMap;
    typedef odk::SimpleBiMap<ChannelDataformat::SampleValueType, std::string> SampleValueTypeStringMap;

    static const SampleOccurrenceStringMap SAMPLE_OCCURRENCE_STRING_MAP = {
        {ChannelDataformat::SampleOccurrence::SYNC, "SYNC"},
        {ChannelDataformat::SampleOccurrence::ASYNC, "ASYNC"},
        {ChannelDataformat::SampleOccurrence::SINGLE_VALUE, "SINGLE_VALUE"},
        {ChannelDataformat::SampleOccurrence::NEVER, "NEVER"},
    };

    static const SampleFormatStringMap SAMPLE_FORMAT_STRING_MAP = {
        {ChannelDataformat::SampleFormat::NONE, "none"},
        {ChannelDataformat::SampleFormat::BYTE, "byte"},
        {ChannelDataformat::SampleFormat::SINT8, "sint8"},
        {ChannelDataformat::SampleFormat::SINT16, "sint16"},
        {ChannelDataformat::SampleFormat::SINT24, "sint24"},
        {ChannelDataformat::SampleFormat::SINT32, "sint32"},
        {ChannelDataformat::SampleFormat::SINT64, "sint64"},
        {ChannelDataformat::SampleFormat::UINT8, "uint8"},
        {ChannelDataformat::SampleFormat::UINT16, "uint16"},
        {ChannelDataformat::SampleFormat::UINT32, "uint32"},
        {ChannelDataformat::SampleFormat::UINT64, "uint64"},
        {ChannelDataformat::SampleFormat::FLOAT, "float"},
        {ChannelDataformat::SampleFormat::DOUBLE, "double"},
        {ChannelDataformat::SampleFormat::CAN_MESSAGE, "can"},
        {ChannelDataformat::SampleFormat::FLEXRAY_MESSAGE, "flexray"},
        {ChannelDataformat::SampleFormat::COMPLEX_FLOAT, "cfloat"},
        {ChannelDataformat::SampleFormat::COMPLEX_DOUBLE, "cdouble"},
        {ChannelDataformat::SampleFormat::VIDEO_RAW_FRAME, "rvideo"},
        {ChannelDataformat::SampleFormat::UTF8_STRING, "utf8str"},
    };

    ASSERT_BIMAP_SIZE(SAMPLE_FORMAT_STRING_MAP, static_cast<size_t>(ChannelDataformat::SampleFormat::RES_NUM_TYPES));

    static const SampleReducedFormatStringMap SAMPLE_REDUCED_FORMAT_STRING_MAP = {
        {ChannelDataformat::SampleReducedFormat::R_R_R, "r_r_r"},
        {ChannelDataformat::SampleReducedFormat::R_SF_SF, "r_sf_sf"},
    };

    static const SampleValueTypeStringMap SAMPLE_VALUE_TYPE_STRING_MAP = {
        {ChannelDataformat::SampleValueType::SAMPLE_VALUE_SCALAR, "scalar"},
        {ChannelDataformat::SampleValueType::SAMPLE_VALUE_CAN_MESSAGE, "can_message"},
        {ChannelDataformat::SampleValueType::SAMPLE_VALUE_VECTOR, "vector"},
        {ChannelDataformat::SampleValueType::SAMPLE_VALUE_COMPLEX_VECTOR, "complex_vector"},
        {ChannelDataformat::SampleValueType::SAMPLE_VALUE_VIDEO, "video"},
        {ChannelDataformat::SampleValueType::SAMPLE_VALUE_STRING, "string"},
        {ChannelDataformat::SampleValueType::SAMPLE_VALUE_FLEXRAY_MESSAGE, "flexray_message"},
        {ChannelDataformat::SampleValueType::SAMPLE_VALUE_BYTE_VECTOR, "byte_vector"},
    };

    static const char* const XML_NAME_DATAFORMAT = "DataFormat";
    static const char* const XML_NAME_SAMPLE_OCCURRENCE = "sample_occurrence";
    static const char* const XML_NAME_SAMPLE_FORMAT = "sample_format";
    static const char* const XML_NAME_SAMPLE_DIMENSION = "sample_dimension";
    static const char* const XML_NAME_REDUCED_FORMAT = "reduced_format";
    static const char* const XML_NAME_SAMPLE_VALUE_TYPE = "sample_value_type";

    namespace
    {
        template <class M, class D>
        bool mapAttributeToValue(const M& map, pugi::xml_node node, const char* const attribute_name, D& value)
        {
            const auto attribute_val = node.attribute(attribute_name).as_string();

            try
            {
                return map.getLeft(attribute_val, value);
            }
            catch (std::out_of_range&)
            {
                return false;
            }
        }
    }

    bool ChannelDataformat::store(pugi::xml_node parent_node) const
    {
        if (m_sample_format != SampleFormat::INVALID)
        {
            // DMD: <DataFormat frequency="SYNC" sample_format="double" reduced_format="r_sf_sf" sample_dimension="1" />
            auto format_node = parent_node.append_child(XML_NAME_DATAFORMAT);
            format_node.append_attribute(XML_NAME_SAMPLE_OCCURRENCE).set_value(SAMPLE_OCCURRENCE_STRING_MAP.getRight(m_sample_occurrence).c_str());
            format_node.append_attribute(XML_NAME_SAMPLE_FORMAT).set_value(SAMPLE_FORMAT_STRING_MAP.getRight(m_sample_format).c_str());
            format_node.append_attribute(XML_NAME_SAMPLE_DIMENSION).set_value(m_sample_dimension);
            if (m_sample_reduced_format != SampleReducedFormat::UNKNOWN)
            {
                format_node.append_attribute(XML_NAME_REDUCED_FORMAT).set_value(SAMPLE_REDUCED_FORMAT_STRING_MAP.getRight(m_sample_reduced_format).c_str());
            }
            if (m_sample_value_type != SampleValueType::SAMPLE_VALUE_INVALID)
            {
                format_node.append_attribute(XML_NAME_SAMPLE_VALUE_TYPE).set_value(SAMPLE_VALUE_TYPE_STRING_MAP.getRight(m_sample_value_type).c_str());
            }

            return true;
        }
        return false;
    }

    bool ChannelDataformat::extract(pugi::xml_node parent_node)
    {
        const auto dataformat_node = xpugi::getChildNodeByTagName(parent_node, XML_NAME_DATAFORMAT);
        if (!dataformat_node)
        {
            return false;
        }

        return parse(dataformat_node);
    }

    std::string ChannelDataformat::generate() const
    {
        pugi::xml_document doc;
        if (store(doc))
        {
            return xpugi::toXML(doc);
        }
        return{};
    }

    bool ChannelDataformat::parse(pugi::xml_node data_format)
    {
        if (std::strcmp(data_format.name(), XML_NAME_DATAFORMAT) != 0)
        {
            return false;
        }

        bool valid = true;
        valid &= mapAttributeToValue(SAMPLE_OCCURRENCE_STRING_MAP, data_format, XML_NAME_SAMPLE_OCCURRENCE, m_sample_occurrence);
        valid &= mapAttributeToValue(SAMPLE_FORMAT_STRING_MAP, data_format, XML_NAME_SAMPLE_FORMAT, m_sample_format);
        //optional
        mapAttributeToValue(SAMPLE_VALUE_TYPE_STRING_MAP, data_format, XML_NAME_SAMPLE_VALUE_TYPE, m_sample_value_type);
        mapAttributeToValue(SAMPLE_REDUCED_FORMAT_STRING_MAP, data_format, XML_NAME_REDUCED_FORMAT, m_sample_reduced_format);
        m_sample_dimension = data_format.attribute(XML_NAME_SAMPLE_DIMENSION).as_uint(1);

        return valid;
    }

    bool ChannelDataformat::parse(const std::string_view& xml_string)
    {
        pugi::xml_document doc;

        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status != pugi::status_ok)
        {
            return false;
        }
        auto data_format = doc.document_element();
        return parse(data_format);
    }

    std::string ChannelDataformat::getSampleFormatString(SampleFormat f)
    {
        return SAMPLE_FORMAT_STRING_MAP.getRight(f);
    }

    static const char* const XML_NAME_CHANNEL = "Channel";

    static const char* const XML_NAME_ID_ATTRIBUTE = "channel_id";


    bool ChannelDataformatTelegram::store(pugi::xml_node parent_node) const
    {
        auto channel_node = parent_node.append_child(XML_NAME_CHANNEL);
        channel_node.append_attribute(XML_NAME_ID_ATTRIBUTE).set_value(channel_id);

        return data_format.store(channel_node);
    }

    bool ChannelDataformatTelegram::extract(pugi::xml_node parent_node)
    {
        const auto channel_node = xpugi::getChildNodeByTagName(parent_node, XML_NAME_CHANNEL);
        if (!channel_node)
        {
            return false;
        }

        return parse(channel_node);
    }

    std::string ChannelDataformatTelegram::generate() const
    {
        pugi::xml_document doc;
        if (store(doc))
        {
            return xpugi::toXML(doc);
        }
        return {};
    }

    bool ChannelDataformatTelegram::parse(pugi::xml_node channel_node)
    {
        if (std::strcmp(channel_node.name(), XML_NAME_CHANNEL) != 0)
        {
            return false;
        }

        channel_id = channel_node.attribute(XML_NAME_ID_ATTRIBUTE).as_ullong(std::numeric_limits<uint64_t>::max());
        if (channel_id != std::numeric_limits<uint64_t>::max())
        {
            return data_format.extract(channel_node);
        }

        return false;
    }

    bool ChannelDataformatTelegram::parse(const std::string_view& xml_string)
    {
        pugi::xml_document doc;

        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status != pugi::status_ok)
        {
            return false;
        }
        auto dfmt = doc.document_element();
        return parse(dfmt);
    }
}
