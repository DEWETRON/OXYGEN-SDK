// Copyright DEWETRON GmbH 2017-2021
#pragma once

#include "odkapi_pugixml_fwd.h"
#include "odkuni_defines.h"
#include <string>

namespace odk
{
    struct Version
    {
        unsigned m_major;
        unsigned m_minor;

        Version(unsigned major = 0, unsigned minor = 0) noexcept;

        ODK_NODISCARD bool isValid() const noexcept;

        ODK_NODISCARD std::string generate() const;
        ODK_NODISCARD static Version parse(const char* str);

        ODK_NODISCARD bool operator==(const Version& other) const noexcept;
        ODK_NODISCARD bool operator!=(const Version& other) const noexcept;
        ODK_NODISCARD bool operator<=(const Version& other) const noexcept;
        ODK_NODISCARD bool operator>=(const Version& other) const noexcept;
    };

    ODK_NODISCARD Version getProtocolVersion(const pugi::xml_node& node);
    void setProtocolVersion(pugi::xml_node& node, const Version& version);
}
