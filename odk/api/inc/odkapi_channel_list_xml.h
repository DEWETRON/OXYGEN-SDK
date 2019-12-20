// Copyright DEWETRON GmbH 2017
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace odk
{
    /**
     * Helper class for handling xml channel lists in the oxygen api.
     * Each entry contains the mandatory channel id as well as an optional status string.
     * Used by DELETE_MATH_CHANNELS.
     */
    struct ChannelList
    {
        struct ChannelInfo
        {
            explicit ChannelInfo(std::uint64_t id)
                : m_channel_id(id)
                , m_status()
            {}

            template<typename S>
            ChannelInfo(std::uint64_t id, S&& status)
                : m_channel_id(id)
                , m_status(std::forward<S>(status))
            {}

            std::uint64_t m_channel_id;
            std::string m_status;

            bool operator==(const ChannelInfo& other) const
            {
                return m_channel_id == other.m_channel_id
                    && m_status == other.m_status;
            }
        };

        std::vector<ChannelInfo> m_channels;

        void addChannel(std::uint64_t id)
        {
            m_channels.emplace_back(id);
        }
        void addChannel(std::uint64_t id, const std::string& status)
        {
            m_channels.emplace_back(id, status);
        }

        bool parse(const char* xml_string);
        std::string generate() const;
        bool valid(bool with_status) const;
        bool operator==(const ChannelList& other) const
        {
            return m_channels == other.m_channels;
        }
    };
}
