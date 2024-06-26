// Copyright DEWETRON GmbH 2017

#include "odkapi_block_descriptor_xml.h"
#include "odkapi_xml_builder.h"

#include "odkuni_string_util.h"
#include "odkuni_xpugixml.h"

#include <cstring>
#include <sstream>

namespace odk
{
    BlockChannelDescriptor::BlockChannelDescriptor() noexcept
        : m_offset()
        , m_channel_id()
        , m_timestamp()
        , m_duration()
        , m_first_sample_index()
        , m_count()
    {
    }


    BlockDescriptor::BlockDescriptor() noexcept
        : m_stream_id(0)
        , m_data_size(0)
    {
    }

    bool BlockDescriptor::parse(const std::string_view& xml_string)
    {
        if (xml_string.empty())
        {
            return false;
        }

        pugi::xml_document doc;
        m_block_channels.clear();
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
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
        std::ostringstream stream;
        {
            using odk::xml_builder::Attribute;
            odk::xml_builder::Document doc(stream);
            auto block_desc_node = doc.append_child("BlockDescriptor",
                Attribute("stream_id", m_stream_id),
                Attribute("data_size", m_data_size));

            for (const auto& block_channel : m_block_channels)
            {
                block_desc_node.append_child("Channel",
                    Attribute("channel_id", block_channel.m_channel_id),
                    Attribute("offset", block_channel.m_offset),
                    Attribute("count", block_channel.m_count),
                    Attribute("first_sample_index", block_channel.m_first_sample_index),
                    Attribute("timestamp", block_channel.m_timestamp),
                    Attribute("duration", block_channel.m_duration));
            }
        }
        return stream.str();
    }

    DataRegion::DataRegion(std::uint64_t channel_id, const Interval<std::uint64_t>& region)
        : m_channel_id(channel_id)
        , m_region(region)
    {
    }

    BlockListDescriptor::BlockListDescriptor() noexcept
        : m_block_count(0)
        , m_windows()
    {
    }

    bool BlockListDescriptor::parse(const std::string_view& xml_string)
    {
        m_windows.clear();

        if (xml_string.empty())
            return false;

        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            try{

                auto block_list_desc_node = doc.document_element();

                m_block_count = odk::from_string<std::uint32_t>(block_list_desc_node.attribute("block_count").value());

                auto interval_nodes = block_list_desc_node.select_nodes("Intervals/Interval");
                m_windows.reserve(interval_nodes.size());
                for (auto interval_node : interval_nodes)
                {
                    auto a_interval_node = interval_node.node();
                    auto begin = odk::from_string<double>(a_interval_node.attribute("begin").value());
                    auto end = odk::from_string<double>(a_interval_node.attribute("end").value());
                    m_windows.emplace_back(begin, end);
                }

                auto channel_region_nodes = block_list_desc_node.select_nodes("InvalidRegions/DataRegion");
                m_invalid_regions.reserve(channel_region_nodes.size());
                for (auto a_region_node : channel_region_nodes)
                {
                    auto a_region_detail_node = a_region_node.node();
                    auto channel_id = odk::from_string<uint64_t>(a_region_detail_node.attribute("channel_id").value());
                    auto begin = odk::from_string<uint64_t>(a_region_detail_node.attribute("begin").value());
                    auto end = odk::from_string<uint64_t>(a_region_detail_node.attribute("end").value());
                    m_invalid_regions.emplace_back(channel_id, Interval<uint64_t>(begin, end));
                }
            }
            catch (const std::logic_error&)
            {
                return false;
            }
        }
        return true;
    }

    std::string BlockListDescriptor::generate() const
    {
        std::ostringstream stream;
        {
            using odk::xml_builder::Attribute;
            odk::xml_builder::Document doc(stream);
            auto block_list_desc_node = doc.append_child("BlockListDescriptor",
                Attribute("block_count", m_block_count));

            if (!m_windows.empty())
            {
                auto intervals_node = block_list_desc_node.append_child("Intervals");
                for (const auto& interval : m_windows)
                {
                    intervals_node.append_child("Interval",
                        Attribute("begin", interval.m_begin),
                        Attribute("end", interval.m_end));
                }
                if (!m_invalid_regions.empty())
                {
                    auto regions_node = block_list_desc_node.append_child("InvalidRegions");

                    for (const auto& a_region : m_invalid_regions)
                    {
                        regions_node.append_child("DataRegion",
                            Attribute("channel_id", a_region.m_channel_id),
                            Attribute("begin", a_region.m_region.m_begin),
                            Attribute("end", a_region.m_region.m_end));
                    }
                }
            }
        }
        return stream.str();
    }

    bool DataRegions::parse(const std::string_view& xml_string)
    {
        pugi::xml_document doc;
        m_data_regions.clear();

        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            try{

                auto data_regions_node = doc.document_element();

                auto channel_region_nodes = data_regions_node.select_nodes("DataRegion");
                m_data_regions.reserve(channel_region_nodes.size());
                for (auto a_region_node : channel_region_nodes)
                {
                    auto a_region_detail_node = a_region_node.node();
                    auto channel_id = odk::from_string<uint64_t>(a_region_detail_node.attribute("channel_id").value());
                    auto begin = odk::from_string<uint64_t>(a_region_detail_node.attribute("begin").value());
                    auto end = odk::from_string<uint64_t>(a_region_detail_node.attribute("end").value());
                    m_data_regions.emplace_back(channel_id, Interval<uint64_t>(begin, end));
                }
            }
            catch (const std::logic_error&) //invalid_argument, out_of_range
            {
                return false;
            }
        }
        return true;
    }

    std::string DataRegions::generate() const
    {
        std::ostringstream stream;
        {
            using odk::xml_builder::Attribute;
            odk::xml_builder::Document doc(stream);
            auto regions_node = doc.append_child("DataRegions");
            for (const auto& a_region : m_data_regions)
            {
                regions_node.append_child("DataRegion",
                    Attribute("channel_id", a_region.m_channel_id),
                    Attribute("begin", a_region.m_region.m_begin),
                    Attribute("end", a_region.m_region.m_end));
            }
        }
        return stream.str();
    }
}

