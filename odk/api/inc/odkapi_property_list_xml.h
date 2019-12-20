// Copyright DEWETRON GmbH 2014

#pragma once

#include "odkapi_node_list_xml.h"
#include "odkapi_property_xml.h"

namespace odk
{
    /**
     * @brief The PropertyList class has a list of Properties than can be converted
     * from/to XML
     *
     * \sa Properties
     */
    class PropertyList : public NodeList<Property>
    {
    public:
        PropertyList();
        PropertyList(std::initializer_list<Property> props);
        Property& getProperty(size_t idx);
        const Property& getProperty(size_t idx) const;
        Property getPropertyByName(const std::string& name) const;
        bool containsProperty(const std::string& name) const;
        PropertyList& setProperty(const Property& p);

        bool operator==(const PropertyList& other) const;
    };
} // namespace neoncfg
