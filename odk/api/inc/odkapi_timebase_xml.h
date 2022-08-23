// Copyright DEWETRON GmbH 2019

#pragma once

#include "odkapi_types.h"

#include "odkuni_defines.h"
#include "odkuni_xpugixml_fwd.h"

#include <string>

namespace odk
{

    struct Timebase : equality_comparable<Timebase>
    {
        enum class TimebaseType
        {
            NONE,
            SIMPLE,
            TIMEBASE_WITH_OFFSET
        };

        //create a Timebase of type NONE
        Timebase() noexcept;

        //create a SIMPLE Timebase with a frequency
        explicit Timebase(double frequency) noexcept;

        //create a Timebase with a frequency and a offset
        Timebase(double frequency, double offset) noexcept;

        //parse any of the supported timebase nodes
        bool parse(pugi::xml_node node);

        //read timebase  with offset information from the parent element
        bool parseWithOffset(pugi::xml_node node);

        //read timebase information from the parent element
        bool extract(pugi::xml_node parent_node);

        //write timebase information as child of parent element
        bool store(pugi::xml_node parent_node) const;

        //generate an XML representation as string
        ODK_NODISCARD std::string generate() const;

        ODK_NODISCARD bool operator==(const Timebase& other) const;

        void reset();

        TimebaseType m_type;
        double m_frequency;
        double m_offset;
    };

}
