// Copyright DEWETRON GmbH 2018

#include "odkapi_channel_mapping_xml.h"
#include "odkuni_assert.h"

#include "odkuni_xpugixml.h"

#include <cstdint>

namespace
{
    template <typename T>
    T readAttributeInt(const pugi::xml_attribute& attr);

    template <>
    std::uint32_t readAttributeInt<std::uint32_t>(const pugi::xml_attribute& attr)
    {
        return attr.as_uint();
    }

    template <>
    std::uint64_t readAttributeInt<std::uint64_t>(const pugi::xml_attribute& attr)
    {
        return attr.as_ullong();
    }

    template <typename IdType>
    bool toLocalChannelIDMap(const pugi::xml_node& node,
        typename odk::ChannelMappingTelegram<IdType>::MapType& channel_id_map)
    {
        if (0 == std::strcmp("ChannelIDMap", node.name()))
        {
            auto channel_mapping_nodes = node.select_nodes("ChannelMappingPair");
            for (const auto channel_mapping_node : channel_mapping_nodes)
            {
                auto first = readAttributeInt<IdType>(channel_mapping_node.node().attribute("first"));
                auto second = readAttributeInt<IdType>(channel_mapping_node.node().attribute("second"));
                ODK_ASSERT_LTE(first, std::numeric_limits<IdType>::max());
                ODK_ASSERT_LTE(second, std::numeric_limits<IdType>::max());
                channel_id_map[first] = second;
            }

            return true;
        }

        return false;
    }

    template <typename IdType>
    bool toLocalChannelIDMapXML(const typename odk::ChannelMappingTelegram<IdType>::MapType& channel_id_map,
        pugi::xml_node& node)
    {
        if (node) // <-- parent
        {
            pugi::xml_node channel_id_map_node = node;

            // lazy create parent
            if (0 != std::strcmp(node.name(), "ChannelIDMap"))
            {
                channel_id_map_node = node.append_child("ChannelIDMap");
            }

            for (const auto& map_entry : channel_id_map)
            {
                auto mapping_entry = channel_id_map_node.append_child("ChannelMappingPair");
                mapping_entry.append_attribute("first").set_value(std::to_string(map_entry.first).c_str());
                mapping_entry.append_attribute("second").set_value(std::to_string(map_entry.second).c_str());
            }

            return true;
        }

        return false;
    }
}

namespace odk
{

    template <class IdType>
    ChannelMappingTelegram<IdType>::ChannelMappingTelegram()
        : m_channel_id_map()
    {
    }

    template <class IdType>
    bool ChannelMappingTelegram<IdType>::parse(const char* xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        auto root_node = doc.document_element();

        return toLocalChannelIDMap<IdType>(root_node, m_channel_id_map);
    }

    template <class IdType>
    std::string ChannelMappingTelegram<IdType>::generate() const
    {
        pugi::xml_document doc;
        auto root_node = doc.append_child("ChannelIDMap");
        if (toLocalChannelIDMapXML<IdType>(m_channel_id_map, root_node))
        {
            return xpugi::toXML(doc);
        }
        return {};
    }

    template class ChannelMappingTelegram<std::uint32_t>;
    template class ChannelMappingTelegram<std::uint64_t>;
}
