// Copyright DEWETRON GmbH 2017

#include "odkapi_data_set_xml.h"
#include "odkapi_version_xml.h"
#include "odkapi_xml_builder.h"

#include "odkuni_string_util.h"
#include "odkuni_xpugixml.h"

#include <sstream>

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

    bool PluginDataSet::parse(const std::string_view& xml_string)
    {
        pugi::xml_document doc;
        m_channels.clear();
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto data_set_node = doc.document_element();

                m_id = odk::from_string<std::uint64_t>(data_set_node.attribute("data_set_key").value());

                auto version = odk::getProtocolVersion(data_set_node);
                if (version != odk::Version(1,0))
                {
                    return false;
                }
                m_data_set_type = stringToDataSetType(data_set_node.attribute("type").value());
                auto channel_nodes = data_set_node.select_nodes("Channels/Channel");
                for (auto channel_node : channel_nodes)
                {
                    auto a_channel_node = channel_node.node();

                    m_channels.push_back(odk::from_string<std::uint64_t>(a_channel_node.attribute("channel_id").value()));
                }

                m_data_mode = stringToDataSetMode(data_set_node.attribute("mode").value());
                m_policy = stringToStreamPolicy(data_set_node.attribute("policy").value());

            }
            catch (const std::logic_error&)
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

    bool PluginDataRequest::parse(const std::string_view&  xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto data_request_node = doc.document_element();
                m_id = odk::from_string<std::uint64_t>(data_request_node.attribute("data_set_key").value());

                if(auto window_node = data_request_node.child("Window"))
                {
                    m_data_window = DataWindow(
                                odk::from_string<double>(window_node.attribute("start").value()),
                                odk::from_string<double>(window_node.attribute("end").value()));
                }

                if(auto single_value_node = data_request_node.child("SingleValue"))
                {
                    m_single_value = SingleValue(
                                odk::from_string<double>(single_value_node.attribute("timestamp").value()));
                }

                if(auto data_stream_node = data_request_node.child("DataStream"))
                {
                    m_data_stream = DataStream();
                }
            }
            catch (const std::logic_error&)
            {
                return false;
            }
        }
        return true;
    }

    std::string PluginDataRequest::generate() const
    {
        std::ostringstream stream;

        {
            using odk::xml_builder::Attribute;
            odk::xml_builder::Document doc(stream);
            auto data_request_node = doc.append_child("DataTransferRequest",
                Attribute("data_set_key", m_id));

            if (m_data_window)
            {
                data_request_node.append_child("Window",
                    Attribute("start", m_data_window->m_start),
                    Attribute("end", m_data_window->m_stop));
            }

            if (m_single_value)
            {
                data_request_node.append_child("SingleValue",
                    Attribute("timestamp", m_single_value->m_timestamp));
            }

            if (m_data_stream)
            {
                data_request_node.append_child("DataStream");
            }
        }

        return stream.str();
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

    bool PluginDataStartRequest::parse(const std::string_view&  xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto data_request_node = doc.document_element();
                m_id = odk::from_string<std::uint64_t>(data_request_node.attribute("data_set_key").value());

                auto window_node = data_request_node.child("Stream");

                m_start = std::nullopt;
                m_block_duration = std::nullopt;

                if (auto start_attr = window_node.attribute("start"))
                {
                    m_start = odk::from_string<double>(start_attr.value());
                }

                if(auto duration_attr = window_node.attribute("block_duration"))
                {
                    m_block_duration = odk::from_string<double>(duration_attr.value());
                }

                if (auto ignore_regions_attr = window_node.attribute("ignore_regions"))
                {
                    m_ignore_regions = ignore_regions_attr.as_bool();
                }

                m_stream_type = stringToStreamType(window_node.attribute("stream_type").value());
            }
            catch (const std::logic_error&)
            {
                return false;
            }
        }
        return true;
    }

    std::string PluginDataStartRequest::generate() const
    {
        std::ostringstream stream;
        {
            using odk::xml_builder::Attribute;
            odk::xml_builder::Document doc(stream);
            auto data_request_node = doc.append_child("DataTransferRequest",
                Attribute("data_set_key", m_id));
            auto stream_node = data_request_node.append_child("Stream");
            if (m_start) { stream_node.append_attribute("start", *m_start); }
            if (m_block_duration) { stream_node.append_attribute("block_duration", *m_block_duration); }
            if (m_ignore_regions) { stream_node.append_attribute("ignore_regions", *m_ignore_regions); }
            stream_node.append_attribute("stream_type", streamTypeToString(m_stream_type));
        }

        return stream.str();
    }


    PluginDataStopRequest::PluginDataStopRequest()
        : m_id(std::numeric_limits<std::uint64_t>::max())
    {}

    PluginDataStopRequest::PluginDataStopRequest(std::uint64_t id)
        : m_id(id)
    {}

    bool PluginDataStopRequest::parse(const std::string_view&  xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto data_request_node = doc.document_element();
                m_id = odk::from_string<std::uint64_t>(data_request_node.attribute("data_set_key").value());
            }
            catch (const std::logic_error&)
            {
                return false;
            }
        }
        return true;
    }

    std::string PluginDataStopRequest::generate() const
    {
        std::ostringstream stream;
        {
            using odk::xml_builder::Attribute;
            odk::xml_builder::Document doc(stream);
            doc.append_child("DataTransferRequest", Attribute("data_set_key", m_id));
        }
        return stream.str();
    }

    PluginDataRegionsRequest::PluginDataRegionsRequest()
        : m_id(std::numeric_limits<std::uint64_t>::max())
    {}

    PluginDataRegionsRequest::PluginDataRegionsRequest(std::uint64_t id)
        : m_id(id)
    {
    }

    bool PluginDataRegionsRequest::parse(const std::string_view& xml_string)
    {
        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            try
            {
                auto data_request_node = doc.document_element();
                m_id = odk::from_string<std::uint64_t>(data_request_node.attribute("data_set_key").value());

                if(auto window_node = data_request_node.child("Window"))
                {
                    m_data_window = DataWindow(
                        odk::from_string<double>(window_node.attribute("start").value()),
                        odk::from_string<double>(window_node.attribute("end").value()));
                }
            }
            catch (const std::logic_error&)
            {
                return false;
            }
        }
        return true;
    }

    std::string PluginDataRegionsRequest::generate() const
    {
        std::ostringstream stream;
        {
            using odk::xml_builder::Attribute;
            odk::xml_builder::Document doc(stream);
            auto data_request_node = doc.append_child("DataRegionsRequest", Attribute("data_set_key", m_id));
            if(m_data_window)
            {
                data_request_node.append_child("Window",
                    Attribute("start", m_data_window->m_start),
                    Attribute("end", m_data_window->m_stop));
            }
        }
        return stream.str();
    }
}
