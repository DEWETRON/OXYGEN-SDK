// Copyright DEWETRON GmbH 2017

#include "odkapi_data_set_xml.h"
#include "odkapi_version_xml.h"

#include "odkuni_xpugixml.h"

#include <boost/lexical_cast.hpp>

namespace odk
{
    PluginDataSet::PluginDataSet()
        : m_id()
        , m_channels()
        , m_data_set_type(DataSetType::SCALED)
        , m_data_mode(DataSetMode::NORMAL)
        , m_policy(StreamPolicy::EXACT)
    {}

    PluginDataSet::PluginDataSet(std::uint64_t id, const std::vector<std::uint64_t>& channels, DataSetType type, DataSetMode mode, StreamPolicy policy)
        : m_id(id)
        , m_channels(channels)
        , m_data_set_type(type)
        , m_data_mode(mode)
        , m_policy(policy)
    {

    }

    bool PluginDataSet::parse(const char* xml_string)
    {
        pugi::xml_document doc;
        m_channels.clear();
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto data_set_node = doc.document_element();

                m_id = boost::lexical_cast<std::uint64_t>(data_set_node.attribute("data_set_key").value());

                auto version = odk::getProtocolVersion(data_set_node);
                if (version != odk::Version(1,0))
                {
                    return false;
                }
                m_data_set_type = stringToDataSetType(boost::lexical_cast<std::string>(data_set_node.attribute("type").value()));
                auto channel_nodes = data_set_node.select_nodes("Channels/Channel");
                for (auto channel_node : channel_nodes)
                {
                    auto a_channel_node = channel_node.node();

                    m_channels.push_back(boost::lexical_cast<std::uint64_t>(a_channel_node.attribute("channel_id").value()));
                }

                m_data_mode = stringToDataSetMode(boost::lexical_cast<std::string>(data_set_node.attribute("mode").value()));
                m_policy = stringToStreamPolicy(boost::lexical_cast<std::string>(data_set_node.attribute("policy").value()));

            }
            catch (const boost::bad_lexical_cast&)
            {
                return false;
            }
        }
        return true;
    }

    std::string PluginDataSet::generate() const
    {
        pugi::xml_document doc;
        auto data_set_node = doc.append_child("RegisterDataSet");

        odk::setProtocolVersion(data_set_node, odk::Version(1,0));

        data_set_node.append_attribute("data_set_key").set_value(m_id);

        data_set_node.append_attribute("type").set_value(dataSetTypeToString(m_data_set_type).c_str());

        auto channels_node = data_set_node.append_child("Channels");

        for (const auto& channel : m_channels)
        {
            auto channel_node = channels_node.append_child("Channel");
            channel_node.append_attribute("channel_id").set_value(channel);
        }

        data_set_node.append_attribute("mode").set_value(dataSetModeToString(m_data_mode).c_str());
        data_set_node.append_attribute("policy").set_value(streamPolicyToString(m_policy).c_str());

        return xpugi::toXML(doc);
    }


    PluginDataRequest::PluginDataRequest()
        : m_id(std::numeric_limits<std::uint64_t>::max())
    {}

    PluginDataRequest::PluginDataRequest(std::uint64_t id, DataWindow data_window)
        : m_id(id)
        , m_data_window(data_window)
    {}

    PluginDataRequest::PluginDataRequest(std::uint64_t id, SingleValue single_value)
        : m_id(id)
        , m_single_value(single_value)
    {}

    PluginDataRequest::PluginDataRequest(std::uint64_t id, PluginDataRequest::DataStream data_stream)
        : m_id(id)
        , m_data_stream(data_stream)
    {}

    bool PluginDataRequest::parse(const char *xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto data_request_node = doc.document_element();
                m_id = boost::lexical_cast<std::uint64_t>(data_request_node.attribute("data_set_key").value());

                if(auto window_node = data_request_node.select_node("Window"))
                {
                    m_data_window = DataWindow(
                                boost::lexical_cast<double>(window_node.node().attribute("start").value()),
                                boost::lexical_cast<double>(window_node.node().attribute("end").value()));
                }

                if(auto single_value_node = data_request_node.select_node("SingleValue"))
                {
                    m_single_value = SingleValue(
                                boost::lexical_cast<double>(single_value_node.node().attribute("timestamp").value()));
                }

                if(auto data_stream_node = data_request_node.select_node("DataStream"))
                {
                    m_data_stream = DataStream();
                }
            }
            catch (const boost::bad_lexical_cast&)
            {
                return false;
            }
        }
        return true;
    }

    std::string PluginDataRequest::generate() const
    {
        pugi::xml_document doc;
        auto data_request_node = doc.append_child("DataTransferRequest");

        data_request_node.append_attribute("data_set_key").set_value(m_id);

        if(m_data_window)
        {
            auto window_node = data_request_node.append_child("Window");
            window_node.append_attribute("start").set_value(m_data_window->m_start);
            window_node.append_attribute("end").set_value(m_data_window->m_stop);
        }

        if(m_single_value)
        {
            auto single_value_node = data_request_node.append_child("SingleValue");
            single_value_node.append_attribute("timestamp").set_value(m_single_value->m_timestamp);
        }

        if(m_data_stream)
        {
            data_request_node.append_child("DataStream");
        }

        return xpugi::toXML(doc);
    }


    PluginDataStartRequest::PluginDataStartRequest()
        : m_id(std::numeric_limits<std::uint64_t>::max())
        , m_start()
        , m_block_duration()
        , m_ignore_regions()
        , m_stream_type(odk::StreamType::PUSH)
    {}

    PluginDataStartRequest::PluginDataStartRequest(std::uint64_t id, double start, double block_duration)
        : m_id(id)
        , m_start(start)
        , m_block_duration(block_duration)
        , m_stream_type(odk::StreamType::PUSH)
    {}

    PluginDataStartRequest::PluginDataStartRequest(std::uint64_t id, double block_duration)
        : m_id(id)
        , m_start()
        , m_block_duration(block_duration)
        , m_stream_type(odk::StreamType::PUSH)
    {}

    bool PluginDataStartRequest::parse(const char *xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto data_request_node = doc.document_element();
                m_id = boost::lexical_cast<std::uint64_t>(data_request_node.attribute("data_set_key").value());

                auto window_node = data_request_node.select_node("Stream").node();

                m_start = boost::none;
                m_block_duration = boost::none;

                if (auto start_attr = window_node.attribute("start"))
                {
                    m_start = boost::lexical_cast<double>(start_attr.value());
                }

                if(auto duration_attr = window_node.attribute("block_duration"))
                {
                    m_block_duration = boost::lexical_cast<double>(duration_attr.value());
                }

                if (auto ignore_regions_attr = window_node.attribute("ignore_regions"))
                {
                    m_ignore_regions = ignore_regions_attr.as_bool();
                }

                m_stream_type = stringToStreamType(boost::lexical_cast<std::string>(window_node.attribute("stream_type").value()));
            }
            catch (const boost::bad_lexical_cast&)
            {
                return false;
            }
        }
        return true;
    }

    std::string PluginDataStartRequest::generate() const
    {
        pugi::xml_document doc;
        auto data_request_node = doc.append_child("DataTransferRequest");

        data_request_node.append_attribute("data_set_key").set_value(m_id);

        auto stream_node = data_request_node.append_child("Stream");

        if (m_start) { stream_node.append_attribute("start").set_value(*m_start); }
        if (m_block_duration) { stream_node.append_attribute("block_duration").set_value(*m_block_duration); }
        if (m_ignore_regions) { stream_node.append_attribute("ignore_regions").set_value(*m_ignore_regions); }
        stream_node.append_attribute("stream_type").set_value(streamTypeToString(m_stream_type).c_str());

        return xpugi::toXML(doc);
    }


    PluginDataStopRequest::PluginDataStopRequest()
        : m_id(std::numeric_limits<std::uint64_t>::max())
    {}

    PluginDataStopRequest::PluginDataStopRequest(std::uint64_t id)
        : m_id(id)
    {}

    bool PluginDataStopRequest::parse(const char *xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto data_request_node = doc.document_element();
                m_id = boost::lexical_cast<std::uint64_t>(data_request_node.attribute("data_set_key").value());
            }
            catch (const boost::bad_lexical_cast&)
            {
                return false;
            }
        }
        return true;
    }

    std::string PluginDataStopRequest::generate() const
    {
        pugi::xml_document doc;
        auto data_request_node = doc.append_child("DataTransferRequest");

        data_request_node.append_attribute("data_set_key").set_value(m_id);

        return xpugi::toXML(doc);
    }

    PluginDataRegionsRequest::PluginDataRegionsRequest()
        : m_id(std::numeric_limits<std::uint64_t>::max())
    {}

    PluginDataRegionsRequest::PluginDataRegionsRequest(std::uint64_t id)
        : m_id(id)
    {
    }

    bool PluginDataRegionsRequest::parse(const char *xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto data_request_node = doc.document_element();
                m_id = boost::lexical_cast<std::uint64_t>(data_request_node.attribute("data_set_key").value());

                if(auto window_node = data_request_node.select_node("Window"))
                {
                    m_data_window = DataWindow(
                        boost::lexical_cast<double>(window_node.node().attribute("start").value()),
                        boost::lexical_cast<double>(window_node.node().attribute("end").value()));
                }
            }
            catch (const boost::bad_lexical_cast&)
            {
                return false;
            }
        }
        return true;
    }

    std::string PluginDataRegionsRequest::generate() const
    {
        pugi::xml_document doc;
        auto data_request_node = doc.append_child("DataRegionsRequest");

        data_request_node.append_attribute("data_set_key").set_value(m_id);

        if(m_data_window)
        {
            auto window_node = data_request_node.append_child("Window");
            window_node.append_attribute("start").set_value(m_data_window->m_start);
            window_node.append_attribute("end").set_value(m_data_window->m_stop);
        }

        return xpugi::toXML(doc);
    }

}

