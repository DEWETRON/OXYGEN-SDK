// Copyright DEWETRON GmbH 2020

#include "odkapi_measurement_header_data_xml.h"
#include "odkapi_version_xml.h"

#include "odkuni_assert.h"
#include "odkuni_xpugixml.h"

namespace odk
{

    MeasurementHeaderData::MeasurementHeaderData()
    {
    }

    MeasurementHeaderData::MeasurementHeaderData(const std::vector<MeasurementHeaderData::Field>& dt)
        : m_data(dt)
    {
    }


    bool MeasurementHeaderData::fromXML(const char* xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            auto root = doc.document_element();
            auto nm = root.name();
            for (auto child_node : root.children("Header"))
            {
                auto name_attr = child_node.attribute("name");
                if (!name_attr.empty())
                {
                    auto type_attr = child_node.attribute("type");

                    Field field { name_attr.value(), "", type_attr.value() };
                    auto value_node = (child_node.child("DisplayValue"));
                    if (!value_node.empty())
                    {
                        field.m_value = value_node.first_child().value();
                    }
                    m_data.emplace_back(field);
                }
            }
        }

        return true;
    }

    std::string MeasurementHeaderData::toXML() const
    {
        pugi::xml_document doc;
        auto root_node = doc.append_child("Headers");

        for (const auto& dt : m_data)
        {
            auto header_node = root_node.append_child("Header");
            ODK_ASSERT(header_node);
            header_node.append_attribute("name").set_value(dt.m_name.c_str());
            header_node.append_attribute("type").set_value(dt.m_type.c_str());

            auto value_node = header_node.append_child("DisplayValue");
            ODK_ASSERT(value_node);
            xpugi::setText(value_node, dt.m_value);
            //value_node.append_child(pugi::node_pcdata).set_value(dt.m_value.c_str());
        }
        return xpugi::toXML(doc);
    }

    MeasurementHeaderDataNames::MeasurementHeaderDataNames()
    {
    }

    MeasurementHeaderDataNames::MeasurementHeaderDataNames(const std::set<std::string>& names)
        : m_names(names)
    {
    }

    bool MeasurementHeaderDataNames::fromXML(const char* xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            auto root_node = doc.document_element();
            if (root_node.begin() != root_node.end())
            {
                //auto list_node = *root_node.begin();
                const auto& children = root_node.children("Name");
                for (auto node : children)
                {
                    if (node.type() == pugi::node_element)
                    {
                        m_names.emplace(xpugi::getText(node));
                    }
                }
            }
        }
        return true;
    }

    std::string MeasurementHeaderDataNames::toXML() const
    {
        pugi::xml_document doc;
        auto root_node = doc.append_child("HeaderNames");
        ODK_ASSERT(root_node);

        for (const auto& text : m_names)
        {
            auto item_element = root_node.append_child("Name");
            ODK_ASSERT(item_element);
            xpugi::setText(item_element, text);
        }

        return xpugi::toXML(doc);
    }
}

