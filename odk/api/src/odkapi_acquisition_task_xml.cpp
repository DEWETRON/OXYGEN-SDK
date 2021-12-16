// Copyright DEWETRON GmbH 2019

#include "odkapi_acquisition_task_xml.h"
#include "odkapi_version_xml.h"

#include "odkbase_basic_values.h"

#include "odkuni_xpugixml.h"

#include <boost/lexical_cast.hpp>

namespace odk
{
    AddAcquisitionTaskTelegram::AddAcquisitionTaskTelegram()
        : m_id()
    {}

    bool AddAcquisitionTaskTelegram::parse(const char* xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto acq_task_node = doc.document_element();

                auto version = odk::getProtocolVersion(acq_task_node);
                if (version != odk::Version(1,0))
                {
                    return false;
                }

                m_id = boost::lexical_cast<std::uint64_t>(acq_task_node.attribute("acquisition_task_key").value());

                auto input_channel_nodes = acq_task_node.select_nodes("InputChannels/Channel");
                for (auto channel_node : input_channel_nodes)
                {
                    auto a_channel_node = channel_node.node();
                    auto a_ch_id = a_channel_node.attribute("channel_id").as_ullong();
                    m_input_channels.push_back(a_ch_id);
                }

                auto output_channel_nodes = acq_task_node.select_nodes("OutputChannels/Channel");
                for (auto channel_node : output_channel_nodes)
                {
                    auto a_channel_node = channel_node.node();
                    auto a_ch_id = a_channel_node.attribute("channel_id").as_ullong();
                    m_output_channels.push_back(a_ch_id);
                }
            }
            catch (const boost::bad_lexical_cast&)
            {
                return false;
            }
        }
        return true;
    }

    std::string AddAcquisitionTaskTelegram::generate() const
    {
        pugi::xml_document doc;
        auto acq_task_node = doc.append_child("AcquisitionTaskAdd");

        odk::setProtocolVersion(acq_task_node, odk::Version(1,0));

        acq_task_node.append_attribute("acquisition_task_key").set_value(m_id);

        auto input_channels_node = acq_task_node.append_child("InputChannels");
        for (const auto& channel : m_input_channels)
        {
            auto channel_node = input_channels_node.append_child("Channel");
            channel_node.append_attribute("channel_id").set_value(channel);
        }

        auto output_channels_node = acq_task_node.append_child("OutputChannels");
        for (const auto& channel : m_output_channels)
        {
            auto channel_node = output_channels_node.append_child("Channel");
            channel_node.append_attribute("channel_id").set_value(channel);
        }

        return xpugi::toXML(doc);
    }

    bool AcquisitionTaskProcessTelegram::parse(const char* xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto acq_task_node = doc.document_element();

                auto version = odk::getProtocolVersion(acq_task_node);
                if (version != odk::Version(1,0))
                {
                    return false;
                }

                if(auto start_node = acq_task_node.child("Start"))
                {
                    m_start.parseTickFrequencyAttributes(start_node);
                }

                if(auto end_node = acq_task_node.child("End"))
                {
                    m_end.parseTickFrequencyAttributes(end_node);
                }
            }
            catch (const boost::bad_lexical_cast&)
            {
                return false;
            }
        }
        return true;
    }

    std::string AcquisitionTaskProcessTelegram::generate() const
    {
        pugi::xml_document doc;
        auto acq_task_node = doc.append_child("AcquisitionTaskProcess");

        odk::setProtocolVersion(acq_task_node, odk::Version(1,0));

        {
            auto start_timestamp_node = acq_task_node.append_child("Start");
            m_start.writeTickFrequencyAttributes(start_timestamp_node);
        }

        {
            auto end_timestamp_node = acq_task_node.append_child("End");
            m_end.writeTickFrequencyAttributes(end_timestamp_node);
        }
        return xpugi::toXML(doc);
    }

}

