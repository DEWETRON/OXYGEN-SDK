// Copyright DEWETRON GmbH 2019

#include "odkapi_timebase_xml.h"
#include "odkuni_xpugixml.h"

#include <cmath>
#include <limits>

namespace
{
    static const char* const XML_ELEM_SIMPLE_TIMEBASE = "SimpleTimebase";
    static const char* const XML_ATTR_FREQUENCY = "frequency";
}

namespace odk
{

    Timebase::Timebase()
        : m_type(TimebaseType::NONE)
        , m_frequency(std::numeric_limits<double>::quiet_NaN())
    {
    }

    Timebase::Timebase(double frequency)
        : m_type(TimebaseType::SIMPLE)
        , m_frequency(frequency)
    {
    }

    bool Timebase::extract(pugi::xml_node parent_node)
    {
        const auto timebase_node = xpugi::getChildNodeByTagName(parent_node, XML_ELEM_SIMPLE_TIMEBASE);
        if (!timebase_node)
        {
            reset();
            return false;
        }

        return parse(timebase_node);
    }

    bool Timebase::store(pugi::xml_node parent_node) const
    {
        if (m_type == TimebaseType::SIMPLE)
        {
            auto timebase_node = parent_node.append_child(XML_ELEM_SIMPLE_TIMEBASE);
            timebase_node.append_attribute(XML_ATTR_FREQUENCY).set_value(m_frequency);
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
        if (strcmp(timebase.name(), XML_ELEM_SIMPLE_TIMEBASE) != 0)
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

    bool Timebase::operator==(const Timebase& other) const
    {
        return m_type == other.m_type
            && ((m_frequency == other.m_frequency) || (std::isnan(m_frequency) && std::isnan(other.m_frequency)));
    }

    void Timebase::reset()
    {
        m_type = TimebaseType::NONE;
        m_frequency = std::numeric_limits<double>::quiet_NaN();
    }

}
