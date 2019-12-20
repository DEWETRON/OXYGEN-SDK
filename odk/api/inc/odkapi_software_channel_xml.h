// Copyright DEWETRON GmbH 2019
#pragma once

#include "odkapi_channel_dataformat_xml.h"

#include <string>
#include <vector>

namespace odk
{
    class RegisterSoftwareChannel
    {
    public:
        RegisterSoftwareChannel();

        bool parse(const char* xml_string);

        std::string generate() const;

        std::string m_display_group;
        std::string m_service_name;
        std::string m_display_name;
        std::string m_description;
    };

    class CreateSoftwareChannel
    {
    public:
        bool parse(const char* xml_string);

        std::string generate() const;

        std::string m_service_name;

        struct InputChannelData
        {
            std::uint64_t channel_id;
            ChannelDataformat
                data_format;
        };

        std::vector<InputChannelData> m_all_selected_channels_data;
    };

    class CreateSoftwareChannelResponse
    {
    public:

        bool parse(const char* xml_string);
        std::string generate() const;

    public:

        std::string m_message; //considered an error if no channels are reported; warning otherwise.

        bool m_show_channel_details = false;
        std::uint32_t m_detail_channel;

        std::vector<std::uint32_t> m_channels;
    };

}

