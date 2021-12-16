// Copyright DEWETRON GmbH 2017
#include "odkapi_version_xml.h"
#include "pugixml.hpp"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/lexical_cast.hpp>
#include <vector>

namespace odk
{

    Version getProtocolVersion(const pugi::xml_node& node)
    {
        const auto& version_attribute = node.attribute("protocol_version");
        if (version_attribute.empty())
        {
            return Version(1);
        }
        return Version::parse(version_attribute.value());
    }

    void setProtocolVersion(pugi::xml_node& node, const Version& version)
    {
        node.append_attribute("protocol_version").set_value(version.generate().c_str());
    }

    Version::Version(unsigned major, unsigned minor) noexcept
        : m_major(major)
        , m_minor(minor)
    {
    }

    bool Version::isValid() const noexcept
    {
        return m_major != 0;
    }

    std::string Version::generate() const
    {
        return std::to_string(m_major) + "." + std::to_string(m_minor);
    }

    Version Version::parse(const char* str)
    {
        std::vector<std::string> numbers;
        boost::split(numbers, str, boost::is_any_of("."));

        unsigned major = 0;
        unsigned minor = 0;

        try
        {
            switch (numbers.size())
            {
            case 2:
                minor = boost::lexical_cast<unsigned>(numbers[1]);
                //fall through
            case 1:
                major = boost::lexical_cast<unsigned>(numbers[0]);
                break;
            }
        }
        catch (boost::bad_lexical_cast&)
        {
            return{};
        }

        return Version(major, minor);
    }

    bool Version::operator!=(const Version& other) const noexcept
    {
        return m_major != other.m_major
            || m_minor != other.m_minor;
    }

    bool Version::operator==(const Version& other) const noexcept
    {
        return m_major == other.m_major
            && m_minor == other.m_minor;
    }

    bool Version::operator<=(const Version& other) const noexcept
    {
        return m_major < other.m_major
            || (m_major == other.m_major && m_minor <= other.m_minor);
    }

    bool Version::operator>=(const Version& other) const noexcept
    {
        return m_major > other.m_major
            || (m_major == other.m_major && m_minor >= other.m_minor);
    }

}

