// Copyright DEWETRON GmbH 2019

#include "odkapi_software_channel_xml.h"

#include "odkapi_channel_list_xml.h"
#include "odkapi_version_xml.h"

#include "odkuni_string_util.h"
#include "odkuni_xpugixml.h"

#include <map>
#include <vector>

namespace
{
    bool appendChannelList(xpugi::xml_element elem, const std::vector<odk::InputChannelData>& channels)
    {
        if (channels.empty()) return true;

        odk::ChannelList list_generator;

        for (const auto& a_channel : channels)
        {
            list_generator.addChannel(a_channel.channel_id);
        }

        const auto channel_id_xml_string = list_generator.generate();

        if (channel_id_xml_string.empty())
        {
            return false;
        }

        return (pugi::status_ok == elem.append_buffer(channel_id_xml_string.c_str(), channel_id_xml_string.size()).status);
    }
}
namespace odk
{
    RegisterSoftwareChannel::RegisterSoftwareChannel()
        : m_analysis_capable(false)
        , m_acquisition_capable(true)
        , m_is_licensed(true)
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

        m_service_name = xpugi::getText(node.child("ServiceName"));
        m_display_name = xpugi::getText(node.child("DisplayName"));
        m_description = xpugi::getText(node.child("Description"));
        m_display_group = xpugi::getText(node.child("DisplayGroup"));

        m_analysis_capable = [node]() {
            if (auto analysis_capability = node.child("AnalysisCapable"))
            {
                return xpugi::getText(analysis_capability) == "True";
            }
            return false;
        }();

        m_acquisition_capable = [node]() {
            if (auto acquisition_capability = node.child("AcquisitionCapable"))
            {
                return xpugi::getText(acquisition_capability) == "True";
            }
            return true; // backwards compatibility with < 5.5
        }();

        m_is_licensed = [node]() {
            if (const auto licensed = node.child("IsLicensed"))
            {
                return xpugi::getText(licensed) == "True";
            }
            return true; // backwards compatibility with < 5.7
        }();

        if (auto ui_add_node = node.child("UIAdd"))
        {
            m_ui_item_add = xpugi::getText(ui_add_node.child("ItemName"));
        }
        else
        {
            m_ui_item_add.clear();
        }

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
        xpugi::setText(xpugi::xml_element(register_elem.append_child("AnalysisCapable")), m_analysis_capable ? "True" : "False");
        xpugi::setText(xpugi::xml_element(register_elem.append_child("AcquisitionCapable")), m_acquisition_capable ? "True" : "False");
        xpugi::setText(xpugi::xml_element(register_elem.append_child("IsLicensed")), m_is_licensed ? "True" : "False");

        if (!m_description.empty())
        {
            xpugi::setText(xpugi::xml_element(register_elem.append_child("Description")), m_description);
        }

        if (!m_ui_item_add.empty())
        {
            auto ui_full_node = register_elem.append_child("UIAdd");
            xpugi::setText(xpugi::xml_element(ui_full_node.append_child("ItemName")), m_ui_item_add);
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

        auto props = node.select_nodes("Property");
        for (const auto& prop : props)
        {
            odk::Property p;
            if (p.readFrom(prop.node(), {1, 0}))
            {
                m_properties.push_back(p);
            }
        }

        const auto channels_node = xpugi::getChildNodeByTagName(node, "Channels");
        if (channels_node)
        {
            odk::ChannelList list_telegram;
            list_telegram.parse(xpugi::toXML(channels_node).c_str());
            for (const auto& a_channel : list_telegram.m_channels)
            {
                auto channel_id_str = odk::to_string(a_channel.m_channel_id);
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


        if (!appendChannelList(register_elem, m_all_selected_channels_data))
        {
            return {};
        }

        if (const auto channels_node = register_elem.child("Channels"))
        {
            for (const auto& a_channel : m_all_selected_channels_data)
            {
                auto channel_id_str = odk::to_string(a_channel.channel_id);
                std::string xpath = "Channel[@channel_id=\"" + channel_id_str + "\"]";
                auto ch_node = channels_node.select_node(xpath.c_str());
                if (!ch_node)
                {
                    return {};
                }

                if (!a_channel.data_format.store(ch_node.node()))
                {
                    return {};
                }
            }
        }

        for (const auto& prop : m_properties)
        {
            prop.appendTo(register_elem);
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

        m_message = xpugi::getText(node.child("Message"));

        bool success = true;
        if (const auto channels_node = node.child("CreatedChannels"))
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

    bool QuerySoftwareChannelAction::parse(const char* xml_string)
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

        if (!odk::strequal(node.name(), "QuerySoftwareChannelAction"))
        {
            return false;
        }

        auto version = odk::getProtocolVersion(node);
        if (version != odk::Version(1,0))
        {
            return false;
        }

        const auto channels_node = xpugi::getChildNodeByTagName(node, "Channels");
        if (channels_node)
        {
            odk::ChannelList list_telegram;
            list_telegram.parse(xpugi::toXML(channels_node).c_str());
            for (const auto& a_channel : list_telegram.m_channels)
            {
                auto channel_id_str = odk::to_string(a_channel.m_channel_id);
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

    std::string QuerySoftwareChannelAction::generate() const
    {
        pugi::xml_document doc;

        auto register_elem = xpugi::xml_element(doc.append_child("QuerySoftwareChannelAction"));
        odk::setProtocolVersion(register_elem, odk::Version(1,0));

        if (!appendChannelList(register_elem, m_all_selected_channels_data))
        {
            return "";
        }

        if (auto channels_node = register_elem.child("Channels"))
        {
            for (const auto& a_channel : m_all_selected_channels_data)
            {
                auto channel_id_str = odk::to_string(a_channel.channel_id);
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

    bool QuerySoftwareChannelActionResponse::parse(const char* xml_string)
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

        if (!odk::strequal(node.name(), "QuerySoftwareChannelActionResponse"))
        {
            return false;
        }

        auto version = odk::getProtocolVersion(node);
        if (version != odk::Version(1,0))
        {
            return false;
        }

        if (auto valid_node = node.child("Valid"))
        {
            m_valid = valid_node.attribute("is_valid").as_bool(false);
        }

        if (const auto channels_node = node.child("Channels"))
        {
            odk::ChannelList list_telegram;
            list_telegram.parse(xpugi::toXML(channels_node).c_str());
            for (const auto& a_channel : list_telegram.m_channels)
            {
                m_invalid_channels.emplace_back(a_channel.m_channel_id);
            }
        }

        return true;
    }

    std::string QuerySoftwareChannelActionResponse::generate() const
    {
        pugi::xml_document doc;

        auto response_elem = xpugi::xml_element(doc.append_child("QuerySoftwareChannelActionResponse"));
        odk::setProtocolVersion(response_elem, odk::Version(1,0));

        auto valid_elem = response_elem.append_child("Valid");
        valid_elem.append_attribute("is_valid").set_value(m_valid);

        if (!m_invalid_channels.empty())
        {
            auto channels_elem = response_elem.append_child("Channels");
            for (const auto& channel : m_invalid_channels)
            {
                auto channel_node = channels_elem.append_child("Channel");

                channel_node.append_attribute("channel_id").set_value(channel);
            }
        }

        return xpugi::toXML(doc);
    }
}
