// Copyright DEWETRON GmbH 2017

#include "odkapi_channel_list_xml.h"
#include "odkapi_version_xml.h"

#include "odkuni_xpugixml.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

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
            if (strcmp(channels_node.name(), "Channels") != 0)
                return false;

            for (const auto channel_node : channels_node.children())
            {
                if (strcmp(channel_node.name(), "Channel") == 0)
                {
                    auto channel_id = channel_node.attribute("channel_id").as_ullong(-1);
                    if (channel_id == -1)
                        return false;
                    std::string status = channel_node.attribute("status").value();
                    m_channels.push_back({channel_id, status});
                }
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

            channel_node.append_attribute("channel_id").set_value(boost::lexical_cast<std::string>(channel.m_channel_id).c_str());
            if (!channel.m_status.empty())
            {
                channel_node.append_attribute("status").set_value(channel.m_status.c_str());
            }
        }

        return xpugi::toXML(doc);
    }

    bool ChannelList::valid(bool with_status) const
    {
        for (const auto& channel : m_channels)
        {
            if (channel.m_channel_id == -1)
                return false;
            if (with_status == channel.m_status.empty())
                return false;
        }
        return true;
    }
}
