// Copyright DEWETRON GmbH 2019
#include "odkapi_channel_config_changed_xml.h"

#include "odkapi_version_xml.h"

#include "odkuni_xpugixml.h"

#include <limits>

namespace odk
{
    ChannelConfigChangedTelegram::ChannelConfig& ChannelConfigChangedTelegram::addChannel(std::uint64_t channel_id)
    {
        m_channel_configs.push_back(ChannelConfig(channel_id));
        return m_channel_configs.back();
    }

    bool ChannelConfigChangedTelegram::parse(const char* xml_string)
    {
        m_channel_configs.clear();

        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            auto request_node = doc.document_element();
            if (std::strcmp(request_node.name(), "ChannelConfigChanged") != 0)
                return false;
            auto version = odk::getProtocolVersion(request_node);
            if (version != odk::Version(1, 0))
                return false;

            for (const auto channel_node : request_node.children())
            {
                if (std::strcmp(channel_node.name(), "Channel") == 0)
                {
                    std::uint64_t chn_id = channel_node.attribute("id").as_ullong(std::numeric_limits<uint64_t>::max());
                    if (chn_id != std::numeric_limits<uint64_t>::max())
                    {
                        auto& ch = addChannel(chn_id);
                        ch.readProperties(channel_node);
                    }
                    else
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    std::string ChannelConfigChangedTelegram::generate() const
    {
        pugi::xml_document doc;
        auto request_node = doc.append_child("ChannelConfigChanged");
        odk::setProtocolVersion(request_node, odk::Version(1, 0));

        for (const auto& ch : m_channel_configs)
        {
            auto channel_node = request_node.append_child("Channel");

            channel_node.append_attribute("id").set_value(ch.m_channel_id);

            ch.appendProperties(channel_node);
        }
        return xpugi::toXML(doc);
    }

    void ChannelConfigChangedTelegram::ChannelConfig::appendProperties(pugi::xml_node channel_node) const
    {
        for (const auto& prop : m_properties)
        {
            auto prop_node = prop.appendTo(channel_node);
        }
    }

    bool ChannelConfigChangedTelegram::ChannelConfig::readProperties(pugi::xml_node channel_node)
    {

        for (const auto property_node : channel_node.children("Property"))
        {
            std::string prop_name = property_node.attribute("name").as_string();
            if (prop_name.empty())
            {
                return false;
            }

            for (const auto property_child_node : property_node.children())
            {
                if (property_child_node.type() != pugi::node_element)
                {
                    //skip
                }
                Property prop(prop_name);
                if (prop.readValue(property_child_node, Version()))
                {
                    addProperty(prop);
                }
                else
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool ChannelConfigChangedTelegram::operator==(const ChannelConfigChangedTelegram& other) const
    {
        return m_channel_configs == other.m_channel_configs;
    }
}
