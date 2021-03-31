// Copyright DEWETRON GmbH 2017

#include "odkapi_block_descriptor_xml.h"

#include "odkuni_xpugixml.h"

#include <boost/lexical_cast.hpp>

namespace odk
{
    BlockChannelDescriptor::BlockChannelDescriptor()
        : m_offset()
        , m_channel_id()
        , m_timestamp()
        , m_duration()
        , m_first_sample_index()
        , m_count()
    {
    }


    BlockDescriptor::BlockDescriptor()
    {
    }

    BlockDescriptor::BlockDescriptor(BlockDescriptor&& other)
        : m_stream_id(other.m_stream_id)
        , m_data_size(other.m_data_size)
        , m_block_channels(std::move(other.m_block_channels))
    {
    }


    bool BlockDescriptor::parse(const char* xml_string)
    {
        pugi::xml_document doc;
        m_block_channels.clear();
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            auto block_desc_node = doc.document_element();

            m_stream_id = block_desc_node.attribute("stream_id").as_ullong();
            m_data_size = block_desc_node.attribute("data_size").as_ullong();

            for (auto& channel_node : block_desc_node.children("Channel"))
            {
                BlockChannelDescriptor channel_desc;
                channel_desc.m_channel_id = channel_node.attribute("channel_id").as_ullong();
                channel_desc.m_offset =     channel_node.attribute("offset").as_uint();
                channel_desc.m_count =      channel_node.attribute("count").as_ullong();
                channel_desc.m_first_sample_index = channel_node.attribute("first_sample_index").as_ullong();
                channel_desc.m_timestamp =  channel_node.attribute("timestamp").as_ullong();
                channel_desc.m_duration =   channel_node.attribute("duration").as_ullong();
                m_block_channels.push_back(channel_desc);
            }
        }
        return true;
    }

    std::string BlockDescriptor::generate() const
    {
        pugi::xml_document doc;
        auto block_desc_node = doc.append_child("BlockDescriptor");

        block_desc_node.append_attribute("stream_id").set_value(m_stream_id);
        block_desc_node.append_attribute("data_size").set_value(m_data_size);

        for (const auto& block_channel : m_block_channels)
        {
            auto channel_node = block_desc_node.append_child("Channel");
            channel_node.append_attribute("channel_id").set_value(block_channel.m_channel_id);
            channel_node.append_attribute("offset").set_value(block_channel.m_offset);
            channel_node.append_attribute("count").set_value(block_channel.m_count);
            channel_node.append_attribute("first_sample_index").set_value(block_channel.m_first_sample_index);
            channel_node.append_attribute("timestamp").set_value(block_channel.m_timestamp);
            channel_node.append_attribute("duration").set_value(block_channel.m_duration);
        }

        return xpugi::toXML(doc);
    }

    DataRegion::DataRegion(std::uint64_t channel_id, const Interval<std::uint64_t>& region)
        : m_channel_id(channel_id)
        , m_region(region)
    {
    }

    BlockListDescriptor::BlockListDescriptor()
        : m_block_count(0)
        , m_windows()
    {
    }

    bool BlockListDescriptor::parse(const char* xml_string)
    {
        pugi::xml_document doc;
        m_windows.clear();

        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            try{

                auto block_list_desc_node = doc.document_element();

                m_block_count = boost::lexical_cast<std::uint32_t>(block_list_desc_node.attribute("block_count").value());

                auto interval_nodes = block_list_desc_node.select_nodes("Intervals/Interval");
                m_windows.reserve(interval_nodes.size());
                for (auto interval_node : interval_nodes)
                {
                    auto a_interval_node = interval_node.node();
                    auto begin = boost::lexical_cast<double>(a_interval_node.attribute("begin").value());
                    auto end = boost::lexical_cast<double>(a_interval_node.attribute("end").value());
                    m_windows.emplace_back(begin, end);
                }

                auto channel_region_nodes = block_list_desc_node.select_nodes("InvalidRegions/DataRegion");
                m_invalid_regions.reserve(channel_region_nodes.size());
                for (auto a_region_node : channel_region_nodes)
                {
                    auto a_region_detail_node = a_region_node.node();
                    auto channel_id = boost::lexical_cast<uint64_t>(a_region_detail_node.attribute("channel_id").value());
                    auto begin = boost::lexical_cast<uint64_t>(a_region_detail_node.attribute("begin").value());
                    auto end = boost::lexical_cast<uint64_t>(a_region_detail_node.attribute("end").value());
                    m_invalid_regions.emplace_back(channel_id, Interval<uint64_t>(begin, end));
                }
            }
            catch (const boost::bad_lexical_cast&)
            {
                return false;
            }
        }
        return true;
    }

    std::string BlockListDescriptor::generate() const
    {
        pugi::xml_document doc;
        auto block_list_desc_node = doc.append_child("BlockListDescriptor");
        block_list_desc_node.append_attribute("block_count").set_value(m_block_count);

        if (!m_windows.empty())
        {
            auto intervals_node = block_list_desc_node.append_child("Intervals");

            for (const auto& interval : m_windows)
            {
                auto interval_node = intervals_node.append_child("Interval");
                interval_node.append_attribute("begin").set_value(interval.m_begin);
                interval_node.append_attribute("end").set_value(interval.m_end);
            }
        }
        if (!m_invalid_regions.empty())
        {
            auto regions_node = block_list_desc_node.append_child("InvalidRegions");

            for (const auto& a_region : m_invalid_regions)
            {
                auto channel_region_node = regions_node.append_child("DataRegion");
                channel_region_node.append_attribute("channel_id").set_value(a_region.m_channel_id);
                channel_region_node.append_attribute("begin").set_value(a_region.m_region.m_begin);
                channel_region_node.append_attribute("end").set_value(a_region.m_region.m_end);
            }
        }

        return xpugi::toXML(doc);
    }

    bool DataRegions::parse(const char* xml_string)
    {
        pugi::xml_document doc;
        m_data_regions.clear();

        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            try{

                auto data_regions_node = doc.document_element();

                auto channel_region_nodes = data_regions_node.select_nodes("DataRegion");
                m_data_regions.reserve(channel_region_nodes.size());
                for (auto a_region_node : channel_region_nodes)
                {
                    auto a_region_detail_node = a_region_node.node();
                    auto channel_id = boost::lexical_cast<uint64_t>(a_region_detail_node.attribute("channel_id").value());
                    auto begin = boost::lexical_cast<uint64_t>(a_region_detail_node.attribute("begin").value());
                    auto end = boost::lexical_cast<uint64_t>(a_region_detail_node.attribute("end").value());
                    m_data_regions.emplace_back(channel_id, Interval<uint64_t>(begin, end));
                }
            }
            catch (const boost::bad_lexical_cast&)
            {
                return false;
            }
        }
        return true;
    }

    std::string DataRegions::generate() const
    {
        pugi::xml_document doc;
        auto regions_node = doc.append_child("DataRegions");

        for (const auto& a_region : m_data_regions)
        {
            auto channel_region_node = regions_node.append_child("DataRegion");
            channel_region_node.append_attribute("channel_id").set_value(a_region.m_channel_id);
            channel_region_node.append_attribute("begin").set_value(a_region.m_region.m_begin);
            channel_region_node.append_attribute("end").set_value(a_region.m_region.m_end);
        }

        return xpugi::toXML(doc);
    }
}

