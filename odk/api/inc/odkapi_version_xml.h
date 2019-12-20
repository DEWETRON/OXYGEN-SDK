// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkapi_pugixml_fwd.h"
#include <string>

namespace odk
{

    struct Version
    {
        unsigned m_major;
        unsigned m_minor;

        Version(unsigned major = 0, unsigned minor = 0);

        bool isValid() const;

        std::string generate() const;
        static Version parse(const char* str);

        bool operator==(const Version& other) const;
        bool operator!=(const Version& other) const;
        bool operator<=(const Version& other) const;
        bool operator>=(const Version& other) const;
    };

    Version getProtocolVersion(const pugi::xml_node& node);
    void setProtocolVersion(pugi::xml_node& node, const Version& version);

}
