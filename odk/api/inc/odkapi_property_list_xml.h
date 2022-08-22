// Copyright DEWETRON GmbH 2014

#pragma once

#include "odkapi_node_list_xml.h"
#include "odkapi_property_xml.h"

#include <vector>

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
        explicit PropertyList(const std::vector<Property>& props);

        Property& getProperty(size_t idx);
        const Property& getProperty(size_t idx) const;
        Property getPropertyByName(const std::string& name) const;
        PropertyList& setProperty(const Property& p);
        bool containsProperty(const std::string& name) const;

        void setBool(const std::string& name, bool value);
        bool getBool(const std::string& name) const;
        bool getBool(std::size_t idx) const;

        void setSigned(const std::string& name, std::int64_t value);
        std::int64_t getSigned(const std::string& name) const;
        std::int64_t getSigned(std::size_t idx) const;

        void setUnsigned(const std::string& name, std::uint64_t value);
        std::uint64_t getUnsigned(const std::string& name) const;
        std::uint64_t getUnsigned(std::size_t idx) const;

        void setDouble(const std::string& name, double value);
        double getDouble(const std::string& name) const;
        double getDouble(std::size_t idx) const;

        void setString(const std::string& name, const std::string& value);
        std::string getString(const std::string& name) const;
        std::string getString(std::size_t idx) const;

        void setXmlString(const std::string& name, const std::string& value);
        std::string getXmlString(const std::string& name) const;
        std::string getXmlString(std::size_t idx) const;

        void setScalar(const std::string& name, double value, const std::string& unit);
        odk::Scalar getScalar(const std::string& name) const;
        odk::Scalar getScalar(std::size_t idx) const;

        void setChannelId(const std::string& name, ChannelID id);
        ChannelID getChannelId(const std::string& name) const;
        ChannelID getChannelId(std::size_t idx) const;

        void setPropertyList(const std::string& name, const PropertyList& props);
        PropertyList getPropertyList(const std::string& name) const;
        PropertyList getPropertyList(std::size_t idx) const;

        bool operator==(const PropertyList& other) const;
    };
} // namespace neoncfg
