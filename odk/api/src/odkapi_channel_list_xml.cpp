// Copyright DEWETRON GmbH 2017

#include "odkapi_channel_list_xml.h"
#include "odkapi_version_xml.h"

#include "odkuni_xpugixml.h"

#include <cstring>
#include <limits>

namespace odk
{

    bool ChannelList::parse(const char* xml_string)
    {
        m_channels.clear();

        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            auto channels_node = doc.document_element();
            if (std::strcmp(channels_node.name(), "Channels") != 0)
                return false;

            for (const auto channel_node : channels_node.children("Channel"))
            {
                auto channel_id = channel_node.attribute("channel_id").as_ullong(std::numeric_limits<uint64_t>::max());
                if (channel_id == std::numeric_limits<uint64_t>::max())
                    return false;
                std::string status_str = channel_node.attribute("status").as_string();
                m_channels.emplace_back(channel_id, status_str);
            }
            return true;
        }
        return false;
    }

    std::string ChannelList::generate() const
    {
        pugi::xml_document doc;
        auto request_node = doc.append_child("Channels");

        for (const auto& channel : m_channels)
        {
            auto channel_node = request_node.append_child("Channel");

            channel_node.append_attribute("channel_id") = static_cast<uint64_t>(channel.m_channel_id);
            if (!channel.m_status.empty())
            {
                channel_node.append_attribute("status") = channel.m_status.c_str();
            }
        }

        return xpugi::toXML(doc);
    }

    bool ChannelList::valid(bool with_status) const
    {
        for (const auto& channel : m_channels)
        {
            if (channel.m_channel_id == std::numeric_limits<uint64_t>::max())
                return false;
            if (with_status == channel.m_status.empty())
                return false;
        }
        return true;
    }
}
