// Copyright DEWETRON GmbH 2017
#include "odkapi_version_xml.h"

#include "odkuni_string_util.h"
#include "pugixml.hpp"

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
        std::vector<std::string> numbers = odk::split(std::string(str), '.');

        unsigned major = 0;
        unsigned minor = 0;

        try
        {
            switch (numbers.size())
            {
            case 2:
                minor = odk::from_string<unsigned>(numbers[1]);
                //fall through
            case 1:
                major = odk::from_string<unsigned>(numbers[0]);
                break;
            }
        }
        catch (std::logic_error&)
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

