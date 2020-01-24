// Copyright DEWETRON GmbH 2019
#pragma once

#define ODK_EXTENSION_FUNCTIONS
#include "odkbase_if_host.h"
#include "odkbase_basic_values.h"

#include "odkapi_channel_dataformat_xml.h"
#include "odkapi_property_xml.h"
#include "odkapi_timebase_xml.h"
#include "odkapi_oxygen_queries.h"


#include "odkuni_xpugixml.h"

#include <cstdint>
#include <string>
#include <vector>


namespace odk
{
namespace framework
{

    class InputChannel;
    typedef std::shared_ptr<InputChannel> InputChannelPtr;

    class InputChannel
    {
    public:
        struct InputChannelData
        {
            std::uint64_t m_channel_id;
            odk::ChannelDataformat dataformat;
            odk::Timebase m_time_base;
        };

        InputChannel(odk::IfHost* host, std::uint64_t channel_id,
            odk::ChannelDataformat dataformat = {})
            : m_input_channel_data({channel_id, dataformat, odk::Timebase()})
            , m_host(host)
        {}

        virtual ~InputChannel() {}

        void setChannelId(std::uint64_t id);

        std::uint64_t getChannelId();

        bool isUsable();

        odk::ChannelDataformat getDataFormat();

        odk::Timebase getTimeBase();

        bool updateTimeBase();

        bool updateDataFormat();

        const odk::Range getRange();

        const std::string getUnit();

        const std::string getName();

    private:
        template <class T>
        odk::detail::ApiObjectPtr<const T> getInputChannelParam(std::uint64_t channel_id, const char* const key)
        {
            std::string channel_context = odk::queries::OxygenChannels;
            channel_context += "#";
            channel_context += std::to_string(channel_id);

            return m_host->getValue<T>(channel_context.c_str(), key);
        }

        template <class T>
        odk::detail::ApiObjectPtr<const T> getInputChannelConfigParam(std::uint64_t channel_id, const char* const key)
        {
            std::string channel_context = odk::queries::OxygenChannels;
            channel_context += "#";
            channel_context += std::to_string(channel_id);
            channel_context += "#Config#";
            channel_context += key;


            return m_host->getValue<T>(channel_context.c_str(), "Value");
        }

        std::string extractDataFormat(const odk::IfXMLValue& xml_value) const;

    private:
        InputChannelData m_input_channel_data;
        odk::IfHost* m_host;
    };

}
}
