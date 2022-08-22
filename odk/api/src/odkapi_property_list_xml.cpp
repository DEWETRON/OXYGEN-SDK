// Copyright DEWETRON GmbH 2014

#include "odkapi_property_list_xml.h"

#include <algorithm>

namespace odk
{

    PropertyList::PropertyList()
    {
    }

    PropertyList::PropertyList(std::initializer_list<Property> props)
    {
        for (auto p : props)
        {
            setProperty(p);
        }
    }

    PropertyList::PropertyList(const std::vector<Property>& props)
    {
        for (auto p : props)
        {
            setProperty(p);
        }
    }

    Property& PropertyList::getProperty(size_t idx)
    {
        return getNode(idx);
    }

    const Property& PropertyList::getProperty(size_t idx) const
    {
        return getNode(idx);
    }

    Property PropertyList::getPropertyByName(const std::string& name) const
    {
        const auto match =
                std::find_if(
                    cbegin(), cend(),
                    [&name] (const odk::Property& property)
                    {
                        return property.getName() == name;
                    });
        return match != cend() ? *match : Property { };
    }

    bool PropertyList::containsProperty(const std::string& name) const
    {
        const auto match =
                std::find_if(
                    cbegin(), cend(),
                    [&name] (const odk::Property& property)
                    {
                        return property.getName() == name;
                    });
        return match != cend();
    }

    PropertyList& PropertyList::setProperty(const Property& p)
    {
        if (!p.getName().empty())
        {
            const auto match =
                std::find_if(
                    begin(), end(),
                    [&p](const odk::Property& property)
                    {
                        return property.getName() == p.getName();
                    });
            if (match != end())
            {
                erase(match, match + 1);
            }
        }
        append(p);
        return *this;
    }

    bool PropertyList::operator==(const PropertyList& other) const
    {
        return NodeList<Property>::operator==(other);
    }

    void PropertyList::setBool(const std::string& name, bool value)
    {
        Property prop(name);
        prop.setValue(value);
        setProperty(prop);
    }
    bool PropertyList::getBool(const std::string& name) const
    {
        auto prop = getPropertyByName(name);
        if (prop.getType() == odk::Property::BOOLEAN)
        {
            return prop.getBoolValue();
        }
        return false;
    }
    bool PropertyList::getBool(std::size_t idx) const
    {
        auto prop = getProperty(idx);
        if (prop.getType() == odk::Property::BOOLEAN)
        {
            return prop.getBoolValue();
        }
        return std::numeric_limits<double>::signaling_NaN();
    }

    void PropertyList::setSigned(const std::string& name, std::int64_t value)
    {
        Property prop(name);
        prop.setValue(value);
        setProperty(prop);
    }
    std::int64_t PropertyList::getSigned(const std::string& name) const
    {
        auto prop = getPropertyByName(name);
        if (prop.getType() == odk::Property::INTEGER || prop.getType() == odk::Property::INTEGER64)
        {
            return prop.getInt64Value();
        }
        return 0;
    }
    std::int64_t PropertyList::getSigned(std::size_t idx) const
    {
        auto prop = getProperty(idx);
        if (prop.getType() == odk::Property::INTEGER || prop.getType() == odk::Property::INTEGER64)
        {
            return prop.getInt64Value();
        }
        return 0;
    }

    void PropertyList::setUnsigned(const std::string& name, std::uint64_t value)
    {
        Property prop(name);
        prop.setValue(value);
        setProperty(prop);
    }
    std::uint64_t PropertyList::getUnsigned(const std::string& name) const
    {
        auto prop = getPropertyByName(name);
        if (prop.getType() == odk::Property::UNSIGNED_INTEGER || prop.getType() == odk::Property::UNSIGNED_INTEGER64)
        {
            return prop.getUnsignedInt64Value();
        }
        return 0;
    }
    std::uint64_t PropertyList::getUnsigned(std::size_t idx) const
    {
        auto prop = getProperty(idx);
        if (prop.getType() == odk::Property::UNSIGNED_INTEGER || prop.getType() == odk::Property::UNSIGNED_INTEGER64)
        {
            return prop.getUnsignedInt64Value();
        }
        return 0;
    }

    void PropertyList::setDouble(const std::string& name, double value)
    {
        Property prop(name);
        prop.setValue(value);
        setProperty(prop);
    }
    double PropertyList::getDouble(const std::string& name) const
    {
        auto prop = getPropertyByName(name);
        if (prop.getType() == odk::Property::FLOATING_POINT_NUMBER)
        {
            return prop.getDoubleValue();
        }
        return std::numeric_limits<double>::signaling_NaN();
    }
    double PropertyList::getDouble(std::size_t idx) const
    {
        auto prop = getProperty(idx);
        if (prop.getType() == odk::Property::FLOATING_POINT_NUMBER)
        {
            return prop.getDoubleValue();
        }
        return std::numeric_limits<double>::signaling_NaN();
    }

    void PropertyList::setString(const std::string& name, const std::string& value)
    {
        Property prop(name);
        prop.setValue(value);
        setProperty(prop);
    }
    std::string PropertyList::getString(const std::string& name) const
    {
        auto prop = getPropertyByName(name);
        if (prop.getType() == odk::Property::STRING)
        {
            return prop.getStringValue();
        }
        return {};
    }
    std::string PropertyList::getString(std::size_t idx) const
    {
        auto prop = getProperty(idx);
        if (prop.getType() == odk::Property::STRING)
        {
            return prop.getStringValue();
        }
        return {};
    }

    void PropertyList::setXmlString(const std::string& name, const std::string& value)
    {
        Property prop(name);
        prop.setValue(value, Property::StringFormat::STRING_XML);
        setProperty(prop);
    }
    std::string PropertyList::getXmlString(const std::string& name) const
    {
        auto prop = getPropertyByName(name);
        if (prop.getType() == odk::Property::STRING)
        {
            return prop.getStringValue();
        }
        return {};
    }
    std::string PropertyList::getXmlString(std::size_t idx) const
    {
        auto prop = getProperty(idx);
        if (prop.getType() == odk::Property::STRING)
        {
            return prop.getStringValue();
        }
        return {};
    }

    void PropertyList::setScalar(const std::string& name, double value, const std::string& unit)
    {
        Property prop(name);
        prop.setValue(Scalar(value, unit));
        setProperty(prop);
    }
    odk::Scalar PropertyList::getScalar(const std::string& name) const
    {
        auto prop = getPropertyByName(name);
        if (prop.getType() == odk::Property::SCALAR)
        {
            return prop.getScalarValue();
        }
        return {};
    }
    odk::Scalar PropertyList::getScalar(std::size_t idx) const
    {
        auto prop = getProperty(idx);
        if (prop.getType() == odk::Property::SCALAR)
        {
            return prop.getScalarValue();
        }
        return {};
    }

    void PropertyList::setChannelId(const std::string& name, ChannelID id)
    {
        Property prop(name);
        prop.setChannelIDValue(id);
        setProperty(prop);
    }
    ChannelID PropertyList::getChannelId(const std::string& name) const
    {
        auto prop = getPropertyByName(name);
        if (prop.getType() == odk::Property::CHANNEL_ID)
        {
            return prop.getChannelIDValue();
        }
        return {};
    }
    ChannelID PropertyList::getChannelId(std::size_t idx) const
    {
        auto prop = getProperty(idx);
        if (prop.getType() == odk::Property::CHANNEL_ID)
        {
            return prop.getChannelIDValue();
        }
        return {};
    }

    void PropertyList::setPropertyList(const std::string& name, const PropertyList& props)
    {
        Property prop(name);
        prop.setValue(props);
        setProperty(prop);
    }
    PropertyList PropertyList::getPropertyList(const std::string& name) const
    {
        auto prop = getPropertyByName(name);
        if (prop.getType() == odk::Property::PROPERTY_LIST)
        {
            return prop.getPropertyListValue();
        }
        return {};
    }
    PropertyList PropertyList::getPropertyList(std::size_t idx) const
    {
        auto prop = getProperty(idx);
        if (prop.getType() == odk::Property::PROPERTY_LIST)
        {
            return prop.getPropertyListValue();
        }
        return {};
    }

} // namespace neoncfg
