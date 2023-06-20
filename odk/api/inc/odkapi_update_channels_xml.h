// Copyright DEWETRON GmbH 2019
#pragma once

#include "odkapi_channel_dataformat_xml.h"
#include "odkapi_types.h"
#include "odkapi_timebase_xml.h"
#include "odkapi_update_config_xml.h"
#include "odkuni_defines.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace odk
{
    /**
    * Helper class for
    * Used by .
    */

    struct UpdateChannelsTelegram : equality_comparable<UpdateChannelsTelegram>
    {
    public:
        struct ChannelGroupInfo : equality_comparable<ChannelGroupInfo>
        {
            std::string m_group_name;
            std::uint32_t m_channel_id = std::numeric_limits<uint32_t>::max();

            std::vector<ChannelGroupInfo> m_children;

            ChannelGroupInfo() = default;

            explicit ChannelGroupInfo(const std::string& name)
                : m_group_name(name)
            {
            }
            explicit ChannelGroupInfo(std::uint32_t channel_id)
                : m_channel_id(channel_id)
            {
            }

            ODK_NODISCARD bool isGroup() const
            {
                return !m_group_name.empty();
            }

            ODK_NODISCARD bool isChannel() const
            {
                return m_channel_id != std::numeric_limits<uint32_t>::max();
            }

            ChannelGroupInfo& appendChannel(std::uint32_t local_id)
            {
                m_children.emplace_back(local_id);
                return m_children.back();
            }

            ChannelGroupInfo& appendGroup(const std::string& name)
            {
                m_children.emplace_back(name);
                return m_children.back();
            }

            ODK_NODISCARD bool operator==(const ChannelGroupInfo& other) const
            {
                return m_group_name == other.m_group_name
                    && m_channel_id == other.m_channel_id
                    && m_children == other.m_children;
            }
        };

        struct PluginChannelInfo : equality_comparable<PluginChannelInfo>
        {
            enum class Status
            {
                OKAY,
                UNSUPPORTED_FORMAT
            };

            std::uint32_t m_local_id = std::numeric_limits<uint32_t>::max();

            ChannelDataformat m_dataformat_info;
            Timebase m_timebase;

            std::string m_default_name;
            std::string m_domain;
            bool m_deletable = false;
            std::uint32_t m_local_parent_id = std::numeric_limits<uint32_t>::max();

            bool m_valid = false;

            odk::UpdateConfigTelegram::ChannelConfig m_channel_config;

            explicit PluginChannelInfo(std::uint32_t local_id = std::numeric_limits<uint32_t>::max())
                : m_local_id(local_id)
                , m_channel_config(odk::UpdateConfigTelegram::PluginChannelInfo(local_id))
            {
            }

            PluginChannelInfo& setSampleFormat(
                ChannelDataformat::SampleOccurrence occurrence,
                ChannelDataformat::SampleFormat format, std::uint32_t dimension = 1)
            {
                m_dataformat_info.m_sample_occurrence = occurrence;
                m_dataformat_info.m_sample_format = format;
                m_dataformat_info.m_sample_dimension = dimension;
                return *this;
            }

            PluginChannelInfo& setSimpleTimebase(double f)
            {
                m_timebase = Timebase(f);
                return *this;
            }

            PluginChannelInfo& setTimebaseWithOffset(double f, double offset)
            {
                m_timebase = Timebase(f, offset);
                return *this;
            }

            PluginChannelInfo& setDefaultName(const std::string& name)
            {
                m_default_name = name;
                return *this;
            }

            PluginChannelInfo& setDomain(const std::string& name)
            {
                m_domain = name;
                return *this;
            }

            PluginChannelInfo& setDeletable(bool deletable)
            {
                m_deletable = deletable;
                return *this;
            }

            PluginChannelInfo& setLocalParent(std::uint32_t local_id)
            {
                m_local_parent_id = local_id;
                return *this;
            }

            PluginChannelInfo& setValid(const bool valid)
            {
                m_valid = valid;
                return *this;
            }

            ODK_NODISCARD bool operator==(const PluginChannelInfo& other) const
            {
                return m_local_id == other.m_local_id
                    && m_dataformat_info == other.m_dataformat_info
                    && m_timebase == other.m_timebase
                    && m_default_name == other.m_default_name
                    && m_domain == other.m_domain
                    && m_deletable == other.m_deletable
                    && m_local_parent_id == other.m_local_parent_id
                    && m_valid == other.m_valid
                    && m_channel_config == other.m_channel_config
                    ;
            }

            // TODO: consider making this a central call that MAY also provide Timebase, Range, Unit, Topo with a single call and avoid the need to handle trivial (user-read-only) config items and topo
            // Possible items for generic handling:
            // CHANNEL_CONFIG_KEY_USED, CHANNEL_CONFIG_KEY_VALID, CHANNEL_CONFIG_KEY_DEFAULT_NAME, CHANNEL_CONFIG_KEY_DEFAULT_GROUP, CHANNEL_CONFIG_KEY_DEFAULT_STORED, CHANNEL_CONFIG_KEY_CHANNEL_TYPE, CHANNEL_CONFIG_KEY_SHORT_ID, CHANNEL_CONFIG_KEY_RANGE, CHANNEL_CONFIG_KEY_UNIT, CHANNEL_CONFIG_KEY_CHANNEL_FLAGS, CHANNEL_CONFIG_KEY_SCALING
            // TODO: concept to support subchannels in the future?
        };

        //bool m_replace_all;
        std::vector<PluginChannelInfo> m_channels;
        ChannelGroupInfo m_list_topology;

        UpdateChannelsTelegram::PluginChannelInfo& addChannel(std::uint32_t local_id);
        void appendChannel(const UpdateChannelsTelegram::PluginChannelInfo& channel_info);
        ODK_NODISCARD UpdateChannelsTelegram::PluginChannelInfo* getChannel(std::uint32_t local_id);
        ODK_NODISCARD const UpdateChannelsTelegram::PluginChannelInfo* getChannel(std::uint32_t local_id) const;
        void removeChannel(std::uint32_t local_id);

        bool parse(const std::string_view& xml_string);
        ODK_NODISCARD std::string generate() const;
        ODK_NODISCARD bool operator==(const UpdateChannelsTelegram& other) const;
    };

    std::vector<std::uint32_t> getRootChannels(const UpdateChannelsTelegram& t);

    std::vector<std::uint32_t> getChildrenOfChannel(const UpdateChannelsTelegram& t, std::uint32_t parent_channel_id, bool recursive);

}
