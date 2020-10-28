// Copyright DEWETRON GmbH 2019
#include "odkapi_update_channels_xml.h"

#include "odkapi_version_xml.h"

#include "odkuni_xpugixml.h"


namespace odk
{
namespace
{
    void appendChannelGroupInfo(pugi::xml_node& parent, const UpdateChannelsTelegram::ChannelGroupInfo& g);
    bool parseChannelGroupInfo(const pugi::xml_node& node, UpdateChannelsTelegram::ChannelGroupInfo& g);
    bool parseChannelGroupInfoChildren(const pugi::xml_node& parent, UpdateChannelsTelegram::ChannelGroupInfo& g);

    void appendChannelGroupInfo(pugi::xml_node& parent, const UpdateChannelsTelegram::ChannelGroupInfo& g)
    {
        pugi::xml_node group_node;
        if (g.m_group_name.empty() && g.m_children.empty())
        { //normal channel reference
            group_node = parent.append_child("Channel");
            group_node.append_attribute("local_id").set_value(g.m_channel_id);
        }
        else if (g.m_group_name.empty() && !g.m_children.empty())
        { //channel that acts as a group
            group_node = parent.append_child("Channel");
            group_node.append_attribute("local_id").set_value(g.m_channel_id);
        }
        else
        { //simple group
            group_node = parent.append_child("Group");
            group_node.append_attribute("name").set_value(g.m_group_name.c_str());
        }
        for (const auto& c : g.m_children)
        {
            appendChannelGroupInfo(group_node, c);
        }
    }

    bool parseChannelGroupInfo(const pugi::xml_node& node, UpdateChannelsTelegram::ChannelGroupInfo& g)
    {
        if (strcmp(node.name(), "Channel") == 0)
        {
            std::uint32_t channel_id = node.attribute("local_id").as_uint(std::numeric_limits<uint32_t>::max());
            g = UpdateChannelsTelegram::ChannelGroupInfo(channel_id);
        }
        else if (strcmp(node.name(), "Group") == 0)
        {
            std::string name = node.attribute("name").as_string();
            g = UpdateChannelsTelegram::ChannelGroupInfo(name);
        }
        else
        {
            return false;
        }
        return parseChannelGroupInfoChildren(node, g);
    }

    bool parseChannelGroupInfoChildren(const pugi::xml_node& parent, UpdateChannelsTelegram::ChannelGroupInfo& g)
    {
        bool ret = false;
        for (const auto node : parent.children())
        {
            g.m_children.push_back({});
            if (!parseChannelGroupInfo(node, g.m_children.back()))
            {
                ret = false;
            }
        }
        return ret;
    }
}


    UpdateChannelsTelegram::PluginChannelInfo& UpdateChannelsTelegram::addChannel(std::uint32_t local_id)
    {
        m_channels.push_back(PluginChannelInfo(local_id));
        auto& ch = m_channels.back();
        return ch;
    }

    void UpdateChannelsTelegram::appendChannel(const UpdateChannelsTelegram::PluginChannelInfo& channel_info)
    {
        m_channels.push_back(channel_info);
    }

    UpdateChannelsTelegram::PluginChannelInfo* UpdateChannelsTelegram::getChannel(std::uint32_t local_id)
    {
        for (size_t i = 0; i < m_channels.size(); ++i)
        {
            if (m_channels[i].m_local_id == local_id)
            {
                return &m_channels[i];
            }
        }
        return nullptr;
    }

    const UpdateChannelsTelegram::PluginChannelInfo* UpdateChannelsTelegram::getChannel(std::uint32_t local_id) const
    {
        for (size_t i = 0; i < m_channels.size(); ++i)
        {
            if (m_channels[i].m_local_id == local_id)
            {
                return &m_channels[i];
            }
        }
        return nullptr;
    }

    bool UpdateChannelsTelegram::parse(const char* xml_string)
    {
        m_channels.clear();

        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            auto request_node = doc.document_element();
            if (strcmp(request_node.name(), "UpdatePluginChannels") != 0)
                return false;
            auto version = odk::getProtocolVersion(request_node);
            if (version != odk::Version(1, 0))
                return false;

            for (const auto node : request_node.children("Channel"))
            {
                std::uint32_t local_id = node.attribute("local_id").as_uint(std::numeric_limits<uint32_t>::max());
                if (local_id != -1)
                {
                    auto& ch = addChannel(local_id);

                    auto data_format = node.child("DataFormat");
                    if (data_format)
                    {
                        const auto xml_str(xpugi::toXML(data_format));
                        if (!xml_str.empty())
                        {
                            if (!ch.m_dataformat_info.parse(xml_str.c_str()))
                            {
                                return false;
                            }
                        }
                    }

                    ch.m_timebase.extract(node);

                    auto parent_node = node.child("Parent");
                    if (parent_node)
                    {
                        ch.m_local_parent_id = parent_node.attribute("local_id").as_uint(std::numeric_limits<uint32_t>::max());
                    }

                    ch.m_channel_config.readProperties(node);

                    ch.m_default_name = node.attribute("default_name").as_string();
                    ch.m_domain = node.attribute("domain").as_string();
                    ch.m_deletable = node.attribute("deletable").as_bool(false);
                    ch.m_valid = node.attribute("valid").as_bool(false);
                }
                else
                {
                    return false;
                }
            }

            if (const auto & topo_node = request_node.child("ListTopology"))
            {
                parseChannelGroupInfoChildren(topo_node, m_list_topology);
            }
            return true;
        }
        return false;
    }

    std::vector<UpdateChannelsTelegram::PluginChannelInfo> sortParentsFirst(
        const std::vector<UpdateChannelsTelegram::PluginChannelInfo>& channels)
    {
        std::vector<UpdateChannelsTelegram::PluginChannelInfo> sorted_channels;

        std::size_t last_sorted_size(sorted_channels.size());
        while(sorted_channels.size() != channels.size())
        {
            std::copy_if(channels.cbegin(), channels.cend(), std::back_inserter(sorted_channels),
                [sorted_channels](const UpdateChannelsTelegram::PluginChannelInfo& channel)
                {
                    //not yet sorted and root channel, or already known parent-channel,
                    return
                        (std::find(
                            sorted_channels.begin(),
                            sorted_channels.end(),
                            channel) == sorted_channels.end()
                        ) &&
                        (
                            (channel.m_local_parent_id == std::uint32_t(-1)) ||
                            (std::find_if(
                                sorted_channels.cbegin(),
                                sorted_channels.cend(),
                                [channel](const UpdateChannelsTelegram::PluginChannelInfo& candidate_channel)
                                {
                                    return candidate_channel.m_local_id == channel.m_local_parent_id;
                                }
                                ) != sorted_channels.end())
                        );
                }
            );

            if (last_sorted_size == sorted_channels.size())
            {
                throw std::domain_error("Cyclic dependency between channels detected");
            }
            last_sorted_size = sorted_channels.size();
        }

        return sorted_channels;
    }

    std::string UpdateChannelsTelegram::generate() const
    {
        pugi::xml_document doc;
        auto request_node = doc.append_child("UpdatePluginChannels");
        odk::setProtocolVersion(request_node, odk::Version(1, 0));

        //sort to ensure parents-first
        auto sorted_channels = sortParentsFirst(m_channels);

        for (const auto& ch : sorted_channels)
        {
            auto channel_node = request_node.append_child("Channel");

            channel_node.append_attribute("local_id").set_value(ch.m_local_id);
            if (!ch.m_default_name.empty())
            {
                channel_node.append_attribute("default_name").set_value(ch.m_default_name.c_str());
            }
            if (!ch.m_domain.empty())
            {
                channel_node.append_attribute("domain").set_value(ch.m_domain.c_str());
            }
            if (ch.m_deletable)
            {
                channel_node.append_attribute("deletable").set_value(ch.m_deletable);
            }

            channel_node.append_attribute("valid").set_value(ch.m_valid);

            const auto dataformat_fragment(ch.m_dataformat_info.generate());
            if (!dataformat_fragment.empty())
            {
                channel_node.append_buffer(dataformat_fragment.c_str(), dataformat_fragment.size());
            }

            if (ch.m_timebase.m_type != odk::Timebase::TimebaseType::NONE)
            {
                ch.m_timebase.store(channel_node);
            }

            if (ch.m_local_parent_id != -1)
            {
                auto parent_node = channel_node.append_child("Parent");
                parent_node.append_attribute("local_id").set_value(ch.m_local_parent_id);
            }

            if (!ch.m_channel_config.m_properties.empty())
            {
                ch.m_channel_config.appendProperties(channel_node);
            }
        }

        if (!m_list_topology.m_children.empty())
        {
            auto topo_node = request_node.append_child("ListTopology");
            for (const auto& c : m_list_topology.m_children)
            {
                appendChannelGroupInfo(topo_node, c);
            }
        }

        return xpugi::toXML(doc);
    }
    bool UpdateChannelsTelegram::operator==(const UpdateChannelsTelegram& other) const
    {
        return m_channels == other.m_channels
            && m_list_topology == other.m_list_topology;
    }
    std::vector<std::uint32_t> getRootChannels(const UpdateChannelsTelegram& t)
    {
        std::vector<std::uint32_t> ret;
        for (auto& ch : t.m_channels)
        {
            if (ch.m_local_parent_id == -1)
            {
                ret.push_back(ch.m_local_id);
            }
        }
        return ret;
    }

    void getChildrenOfChannel(const UpdateChannelsTelegram& t, std::uint32_t parent_channel_id, bool recursive, std::vector<std::uint32_t>& children)
    {
        for (auto& ch : t.m_channels)
        {
            if (ch.m_local_parent_id == parent_channel_id)
            {
                children.push_back(ch.m_local_id);

                //emergency abort if cycles exist
                if (children.size() > t.m_channels.size())
                {
                    throw std::domain_error("Cyclic dependency between channels detected");
                }

                if (recursive)
                {
                    getChildrenOfChannel(t, ch.m_local_id, true, children);
                }
            }
        }
    }

    std::vector<std::uint32_t> getChildrenOfChannel(const UpdateChannelsTelegram& t, std::uint32_t parent_channel_id, bool recursive)
    {
        std::vector<std::uint32_t> ret;
        getChildrenOfChannel(t, parent_channel_id, recursive, ret);
        return ret;
    }
}
