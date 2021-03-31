// Copyright DEWETRON GmbH 2020
#pragma once

#include "odkapi_types.h"
#include "odkapi_property_list_xml.h"

#include <boost/optional.hpp>
#include <cstdint>
#include <string>
#include <set>

namespace odk
{
    /**
     * Generates the MeasurementHeaderData from an odk query xml return
     */
    class MeasurementHeaderData
    {
        public:
            struct Field
            {
                std::string m_name;
                std::string m_value;
            };

            MeasurementHeaderData();
            explicit MeasurementHeaderData(const std::vector<Field>&);

            bool fromXML(const char* xml_string);
            std::string toXML() const;

            std::vector<Field> m_data;
    };

    class MeasurementHeaderDataNames
    {
        public:
            MeasurementHeaderDataNames();
            explicit MeasurementHeaderDataNames(const std::set<std::string>&);

            bool fromXML(const char* xml_string);
            std::string toXML() const;

            std::set<std::string> m_names;
    };
}


