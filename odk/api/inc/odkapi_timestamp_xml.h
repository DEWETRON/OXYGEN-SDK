// Copyright DEWETRON GmbH 2018
#pragma once

#include "odkuni_xpugixml_fwd.h"
#include <cstdint>
#include <string>

namespace odk
{
    class Timestamp
    {
    public:
        Timestamp();
        Timestamp(std::uint64_t ticks, double frequency);

        bool parse(const char* xml_string);
        std::string generate() const;

        bool parseTickFrequencyAttributes(const pugi::xml_node& node);
        void writeTickFrequencyAttributes(pugi::xml_node& node) const;

        bool timestampValid() const;

        std::uint64_t m_ticks;
        double m_frequency;
    };
}
