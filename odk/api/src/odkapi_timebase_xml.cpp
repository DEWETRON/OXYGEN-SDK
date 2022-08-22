// Copyright DEWETRON GmbH 2019

#include "odkapi_timebase_xml.h"
#include "odkuni_xpugixml.h"

#include <cmath>
#include <cstring>
#include <limits>

namespace
{
    static const char* const XML_ELEM_SIMPLE_TIMEBASE = "SimpleTimebase";
    static const char* const XML_ELEM_TIMEBASE_WITH_OFFSET = "TimebaseWithOffset";
    static const char* const XML_ATTR_FREQUENCY = "frequency";
    static const char* const XML_ATTR_OFFSET = "offset";
}

namespace odk
{

    Timebase::Timebase()
        : m_type(TimebaseType::NONE)
        , m_frequency(std::numeric_limits<double>::quiet_NaN())
        , m_offset(0)
    {
    }

    Timebase::Timebase(double frequency)
        : m_type(TimebaseType::SIMPLE)
        , m_frequency(frequency)
        , m_offset(0)
    {
    }

    Timebase::Timebase(double frequency, double offset)
        : m_type(TimebaseType::TIMEBASE_WITH_OFFSET)
        , m_frequency(frequency)
        , m_offset(offset)
    {
    }

    bool Timebase::extract(pugi::xml_node parent_node)
    {
        const auto timebase_node = xpugi::getChildNodeByTagName(parent_node, XML_ELEM_SIMPLE_TIMEBASE);
        if (timebase_node)
        {
            return parse(timebase_node);
        }

        const auto timebase_offset_node = xpugi::getChildNodeByTagName(parent_node, XML_ELEM_TIMEBASE_WITH_OFFSET);
        if (timebase_offset_node)
        {
            return parseWithOffset(timebase_offset_node);
        }

        reset();
        return false;
    }

    bool Timebase::store(pugi::xml_node parent_node) const
    {
        if (m_type == TimebaseType::SIMPLE)
        {
            auto timebase_node = parent_node.append_child(XML_ELEM_SIMPLE_TIMEBASE);
            timebase_node.append_attribute(XML_ATTR_FREQUENCY).set_value(m_frequency);
        }
        else if (m_type == TimebaseType::TIMEBASE_WITH_OFFSET)
        {
            auto timebase_node = parent_node.append_child(XML_ELEM_TIMEBASE_WITH_OFFSET);
            timebase_node.append_attribute(XML_ATTR_FREQUENCY).set_value(m_frequency);
            timebase_node.append_attribute(XML_ATTR_OFFSET).set_value(m_offset);
        }

        return true;
    }

    std::string Timebase::generate() const
    {
        pugi::xml_document doc;
        if (store(doc))
        {
            return xpugi::toXML(doc);
        }
        else
        {
            return {};
        }
    }

    bool Timebase::parse(pugi::xml_node timebase)
    {
        reset();
        if (std::strcmp(timebase.name(), XML_ELEM_SIMPLE_TIMEBASE) != 0)
        {
            return false;
        }

        m_frequency = timebase.attribute(XML_ATTR_FREQUENCY).as_double(std::numeric_limits<double>::quiet_NaN());
        if (std::isnan(m_frequency))
        {
            return false;
        }
        m_type = TimebaseType::SIMPLE;

        return true;
    }

    bool Timebase::parseWithOffset(pugi::xml_node timebase)
    {
        reset();
        if (std::strcmp(timebase.name(), XML_ELEM_TIMEBASE_WITH_OFFSET) != 0)
        {
            return false;
        }

        m_offset = timebase.attribute(XML_ATTR_OFFSET).as_double(0.0);
        m_frequency = timebase.attribute(XML_ATTR_FREQUENCY).as_double(std::numeric_limits<double>::quiet_NaN());
        if (std::isnan(m_frequency))
        {
            return false;
        }
        m_type = TimebaseType::TIMEBASE_WITH_OFFSET;

        return true;
    }


    bool Timebase::operator==(const Timebase& other) const
    {
        return m_type == other.m_type
            && ((m_frequency == other.m_frequency) || (std::isnan(m_frequency) && std::isnan(other.m_frequency))
            && (m_offset == other.m_offset));
    }

    void Timebase::reset()
    {
        m_type = TimebaseType::NONE;
        m_frequency = std::numeric_limits<double>::quiet_NaN();
        m_offset = 0;
    }

}
