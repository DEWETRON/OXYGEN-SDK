// Copyright DEWETRON GmbH 2019
#pragma once

#include "odkapi_channel_dataformat_xml.h"
#include "odkapi_property_xml.h"

#include <string>
#include <vector>

namespace odk
{
    struct InputChannelData
    {
        std::uint64_t channel_id;
        ChannelDataformat data_format;
    };

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
        std::string m_ui_item_add;
    };

    class CreateSoftwareChannel
    {
    public:
        bool parse(const char* xml_string);

        std::string generate() const;

        std::string m_service_name;

        std::vector<InputChannelData> m_all_selected_channels_data;
        std::vector<odk::Property> m_properties;
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

    class QuerySoftwareChannelAction
    {
    public:
        bool parse(const char* xml_string);
        std::string generate() const;

    public:
        std::vector<InputChannelData> m_all_selected_channels_data;
    };

    class QuerySoftwareChannelActionResponse
    {
    public:
        bool parse(const char* xml_string);
        std::string generate() const;

    public:
        std::vector<std::uint64_t> m_invalid_channels;
        bool m_valid;
    };

}

