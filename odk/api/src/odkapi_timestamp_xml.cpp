// Copyright DEWETRON GmbH 2017

#include "odkapi_timestamp_xml.h"

#include "odkuni_xpugixml.h"

#include <boost/lexical_cast.hpp>

namespace odk
{
    Timestamp::Timestamp()
        : m_ticks(0)
        , m_frequency(0.0)
    {}

    Timestamp::Timestamp(std::uint64_t ticks, double frequency)
        : m_ticks(ticks)
        , m_frequency(frequency)
    {}

    bool Timestamp::timestampValid() const
    {
        return m_frequency > 0;
    }

    bool Timestamp::parse(const char *xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            auto node = doc.document_element();
            if (!node)
            {
                return false;
            }
            return parseTickFrequencyAttributes(node);
        }
        else
        {
            return false;
        }
    }

    std::string Timestamp::generate() const
    {
        pugi::xml_document doc;
        auto node = doc.append_child("Timestamp");
        writeTickFrequencyAttributes(node);
        return xpugi::toXML(doc);
    }

    void Timestamp::writeTickFrequencyAttributes(pugi::xml_node& node) const
    {
        node.append_attribute("ticks").set_value(m_ticks);
        node.append_attribute("frequency").set_value(m_frequency);
    }

    bool Timestamp::parseTickFrequencyAttributes(const pugi::xml_node& node)
    {
        m_ticks = 0;
        m_frequency = -1;

        try
        {
            auto ticks_attr = node.attribute("ticks");
            auto freq_attr = node.attribute("frequency");
            if (!ticks_attr || !freq_attr)
            {
                return false;
            }

            m_ticks = boost::lexical_cast<std::uint64_t>(ticks_attr.value());
            m_frequency = boost::lexical_cast<double>(freq_attr.value());
            return true;
        }
        catch (const boost::bad_lexical_cast&)
        {
            return false;
        }
    }
}
