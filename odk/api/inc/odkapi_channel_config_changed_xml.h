// Copyright DEWETRON GmbH 2019
#pragma once

#define ODK_EXTENSION_FUNCTIONS //enable C++ integration

#include "odkapi_pugixml_fwd.h"
#include "odkapi_property_xml.h"

#include <string>
#include <vector>

namespace odk
{
    struct ChannelConfigChangedTelegram
    {
    public:
        typedef std::vector<Property> ConfigItemVec_t;

        struct ChannelConfig
        {
            ChannelConfig(const std::uint64_t channel_id)
                : m_channel_id(channel_id)
                , m_properties()
            {}

            ChannelConfig& addProperty(const Property& prop)
            {
                m_properties.push_back(prop);
                return *this;
            }

            bool operator==(const ChannelConfig& other) const
            {
                return m_channel_id == other.m_channel_id
                    && m_properties == other.m_properties;
            }

            void appendProperties(pugi::xml_node channel_node) const;
            bool readProperties(pugi::xml_node channel_node);
            std::uint64_t m_channel_id;
            ConfigItemVec_t m_properties;
        };

        ChannelConfigChangedTelegram::ChannelConfig& addChannel(std::uint64_t channel_id);

        std::vector<ChannelConfig> m_channel_configs;
        bool parse(const char* xml_string);
        std::string generate() const;

        bool operator==(const ChannelConfigChangedTelegram& other) const;
    };
}
