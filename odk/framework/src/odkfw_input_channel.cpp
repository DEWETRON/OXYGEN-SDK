// Copyright DEWETRON GmbH 2019
#include "odkfw_input_channel.h"

namespace odk
{
namespace framework
{
    void InputChannel::setChannelId(std::uint64_t id)
    {
        m_input_channel_data.m_channel_id = id;
    }

    std::uint64_t InputChannel::getChannelId()
    {
        return m_input_channel_data.m_channel_id;
    }

    bool InputChannel::isUsable()
    {
        const auto is_usable = getInputChannelParam<odk::IfBooleanValue>("Usable");
        if (is_usable)
        {
            return is_usable->getValue();
        }

        return false;
    }

    bool InputChannel::isIdValid()
    {
        return getChannelId() != std::numeric_limits<uint64>::max();
    }

    odk::ChannelDataformat InputChannel::getDataFormat()
    {
        return m_input_channel_data.dataformat;
    }

    odk::Timebase InputChannel::getTimeBase()
    {
        return m_input_channel_data.m_time_base;
    }

    bool InputChannel::updateTimeBase()
    {
        const auto timebase_xml = getInputChannelParam<odk::IfXMLValue>("Timebase");

        if (timebase_xml)
        {
            pugi::xml_document doc;
            auto status = doc.load_string(timebase_xml->getValue());
            if (status.status == pugi::status_ok)
            {
                odk::Timebase timebase;
                if (timebase.extract(doc))
                {
                    m_input_channel_data.m_time_base = timebase;
                    return true;
                }
            }
        }
        return false;
    }

    bool InputChannel::updateDataFormat()
    {
        const auto data_format_xml = getInputChannelParam<odk::IfXMLValue>("DataFormat");
        if (!data_format_xml)
        {
            return false;
        }

        odk::ChannelDataformat dataformat_bak(m_input_channel_data.dataformat);

        if (!m_input_channel_data.dataformat.parse(data_format_xml->getValue()))
        {
            //corrupt or legacy xml
            const auto data_format_extracted = extractDataFormat(*data_format_xml);
            if (!m_input_channel_data.dataformat.parse(data_format_extracted.c_str()))
            {
                m_input_channel_data.dataformat = dataformat_bak;
                return false;
            }
        }
        return true;
    }

    const odk::Range InputChannel::getRange()
    {
        if (m_input_channel_data.dataformat.m_sample_value_type ==
            odk::ChannelDataformat::SampleValueType::SAMPLE_VALUE_SCALAR)
        {
            if (const auto range_xml = getInputChannelParam<odk::IfXMLValue>("Range"))
            {
                pugi::xml_document doc;
                auto status = doc.load_string(range_xml->getValue());

                if (status.status == pugi::status_ok)
                {
                    if (const auto range_node = xpugi::getChildNodeByTagName(doc, "ScalarRange"))
                    {
                        if (const auto property_node = xpugi::getChildNodeByTagName(range_node, "RangeValue"))
                        {
                            odk::Property range_property;
                            range_property.readValue(property_node, Version());
                            return range_property.getRangeValue();
                        }
                    }
                }
            }
        }
        else
        {
            if (const auto range_val = getInputChannelConfigParam<odk::IfScalarRange>("Range"))
            {
                odk::Property range_property("Range",
                    odk::Range(range_val->getMin(), range_val->getMax(),
                                    range_val->getMinUnit(), range_val->getMaxUnit()));

                return range_property.getRangeValue();
            }
        }
        return {};
    }

    const std::string InputChannel::getUnit()
    {
        if (const auto unit = getInputChannelParam<odk::IfStringValue>("Unit"))
        {
            return unit->getValue();
        }
        return {};
    }

    const std::string InputChannel::getName()
    {
        if (const auto unit = getInputChannelParam<odk::IfStringValue>("Name"))
        {
            return unit->getValue();
        }
        return {};
    }

    std::string InputChannel::extractDataFormat(const odk::IfXMLValue& xml_value) const
    {
        pugi::xml_document doc;

        auto status = doc.load_string(xml_value.getValue());
        if (status.status == pugi::status_ok)
        {
            auto data_format = doc.document_element();
            std::string xpath = "ChannelDataFormat/Channel/DataFormat";

            auto data_format_node = doc.select_node(xpath.c_str());
            if (data_format_node)
            {
                const auto data_format_xml_str(xpugi::toXML(data_format_node));
                return data_format_xml_str;
            }
        }
        return "";
    }
}
}
