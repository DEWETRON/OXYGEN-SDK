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
        append(p);
        return *this;
    }

    bool PropertyList::operator==(const PropertyList& other) const
    {
        return NodeList<Property>::operator==(other);
    }

} // namespace neoncfg
