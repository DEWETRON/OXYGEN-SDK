// Copyright DEWETRON GmbH 2019

#include "odkapi_software_channel_xml.h"

#include "odkapi_channel_list_xml.h"
#include "odkapi_version_xml.h"

#include "odkuni_string_util.h"
#include "odkuni_xpugixml.h"

#include <map>
#include <vector>

namespace odk
{
    RegisterSoftwareChannel::RegisterSoftwareChannel()
    {}

    bool RegisterSoftwareChannel::parse(const char *xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status != pugi::status_ok)
        {
            return false;
        }

        auto node = doc.document_element();
        if (!node)
        {
            return false;
        }

        if (!odk::strequal(node.name(), "RegisterSoftwareChannel"))
        {
            return false;
        }

        auto version = odk::getProtocolVersion(node);
        if (version != odk::Version(1,0))
        {
            return false;
        }

        m_service_name = xpugi::getText(xpugi::getChildNodeByTagName(node, "ServiceName"));
        m_display_name = xpugi::getText(xpugi::getChildNodeByTagName(node, "DisplayName"));
        m_description = xpugi::getText(xpugi::getChildNodeByTagName(node, "Description"));
        m_display_group = xpugi::getText(xpugi::getChildNodeByTagName(node, "DisplayGroup"));

        return true;
    }

    std::string RegisterSoftwareChannel::generate() const
    {
        pugi::xml_document doc;

        auto register_elem = xpugi::xml_element(doc.append_child("RegisterSoftwareChannel"));
        odk::setProtocolVersion(register_elem, odk::Version(1,0));

        xpugi::setText(xpugi::xml_element(register_elem.append_child("ServiceName")), m_service_name);
        xpugi::setText(xpugi::xml_element(register_elem.append_child("DisplayName")), m_display_name);
        xpugi::setText(xpugi::xml_element(register_elem.append_child("DisplayGroup")), m_display_group);
        if (!m_description.empty())
        {
            xpugi::setText(xpugi::xml_element(register_elem.append_child("Description")), m_description);
        }

        return xpugi::toXML(doc);
    }

    bool CreateSoftwareChannel::parse(const char *xml_string)
    {
        m_all_selected_channels_data.clear();

        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status != pugi::status_ok)
        {
            return false;
        }

        auto node = doc.document_element();
        if (!node)
        {
            return false;
        }

        if (!odk::strequal(node.name(), "CreateSoftwareChannel"))
        {
            return false;
        }

        auto version = odk::getProtocolVersion(node);
        if (version != odk::Version(1,0))
        {
            return false;
        }

        m_service_name = xpugi::getText(xpugi::getChildNodeByTagName(node, "ServiceName"));

        const auto channels_node = xpugi::getChildNodeByTagName(node, "Channels");
        if (channels_node)
        {
            odk::ChannelList list_telegram;
            list_telegram.parse(xpugi::toXML(channels_node).c_str());
            for (const auto& a_channel : list_telegram.m_channels)
            {
                auto channel_id_str = std::to_string(a_channel.m_channel_id);
                std::string xpath = "Channel[@channel_id=\"" + channel_id_str + "\"]";
                auto ch_node = channels_node.select_node(xpath.c_str());
                if (!ch_node)
                {
                    return false;
                }
                ChannelDataformat data_format;
                if (data_format.extract(ch_node.node()))
                {
                    m_all_selected_channels_data.push_back({ a_channel.m_channel_id, data_format });
                }
            }
        }

        return true;
    }

    std::string CreateSoftwareChannel::generate() const
    {
        pugi::xml_document doc;

        auto register_elem = xpugi::xml_element(doc.append_child("CreateSoftwareChannel"));
        odk::setProtocolVersion(register_elem, odk::Version(1,0));

        xpugi::setText(xpugi::xml_element(register_elem.append_child("ServiceName")), m_service_name);

        odk::ChannelList list_generator;

        for (const auto a_channel : m_all_selected_channels_data)
        {
            list_generator.addChannel(a_channel.channel_id);
        }

        pugi::xml_document channel_id_doc;
        const auto channel_id_xml_string = list_generator.generate();

        if (channel_id_xml_string.empty())
        {
            return "";
        }

        register_elem.append_buffer(channel_id_xml_string.c_str(), channel_id_xml_string.size());
        const auto channels_node = xpugi::getChildNodeByTagName(register_elem, "Channels");
        if (channels_node)
        {
            for (const auto a_channel : m_all_selected_channels_data)
            {
                auto channel_id_str = std::to_string(a_channel.channel_id);
                std::string xpath = "Channel[@channel_id=\"" + channel_id_str + "\"]";
                auto ch_node = channels_node.select_node(xpath.c_str());
                if (!ch_node)
                {
                    return "";
                }

                if (!a_channel.data_format.store(ch_node.node()))
                {
                    return "";
                }
            }
        }

        return xpugi::toXML(doc);
    }

    bool CreateSoftwareChannelResponse::parse(const char* xml_string)
    {
        *this = CreateSoftwareChannelResponse();

        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status != pugi::status_ok)
        {
            return false;
        }

        auto node = doc.document_element();
        if (!node)
        {
            return false;
        }

        if (!odk::strequal(node.name(), "CreateSoftwareChannelResponse"))
        {
            return false;
        }

        auto version = odk::getProtocolVersion(node);
        if (version != odk::Version(1, 0))
        {
            return false;
        }

        m_message = xpugi::getText(xpugi::getChildNodeByTagName(node, "Message"));

        bool success = true;
        const auto channels_node = xpugi::getChildNodeByTagName(node, "CreatedChannels");
        if (channels_node)
        {
            for (auto ch : channels_node.select_nodes("Channel"))
            {
                auto attr = ch.node().attribute("channel_id");
                if (attr)
                {
                    m_channels.push_back(attr.as_uint());
                    if (!m_show_channel_details && ch.node().attribute("show_details").as_bool())
                    {
                        m_show_channel_details = true;
                        m_detail_channel = m_channels.back();
                    }
                }
                else
                {
                    success = false;
                }
            }
        }

        return success;
    }

    std::string CreateSoftwareChannelResponse::generate() const
    {
        pugi::xml_document doc;

        auto response_elem = xpugi::xml_element(doc.append_child("CreateSoftwareChannelResponse"));
        odk::setProtocolVersion(response_elem, odk::Version(1, 0));

        if (!m_message.empty())
        {
            xpugi::setText(xpugi::xml_element(response_elem.append_child("Message")), m_message);
        }

        if (!m_channels.empty())
        {
            auto channels_elem = response_elem.append_child("CreatedChannels");
            for (const auto& channel : m_channels)
            {
                auto channel_node = channels_elem.append_child("Channel");

                channel_node.append_attribute("channel_id").set_value(channel);

                if (m_show_channel_details && m_detail_channel == channel)
                {
                    channel_node.append_attribute("show_details").set_value(true);
                }
            }
        }

        return xpugi::toXML(doc);
    }

}
