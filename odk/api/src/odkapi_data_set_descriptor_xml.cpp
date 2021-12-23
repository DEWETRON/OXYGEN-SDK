// Copyright DEWETRON GmbH 2017

#include "odkapi_data_set_descriptor_xml.h"
#include "odkuni_assert.h"

#include "odkuni_xpugixml.h"

#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace odk
{

    namespace
    {
        SampleType parseSampleType(const std::string& type)
        {
            if (boost::iequals(type, "double"))
            {
                return SampleType::DOUBLE;
            }
            else if (boost::iequals(type, "16bit_sint"))
            {
                return SampleType::SIGNED16BIT;
            }
            else if (boost::iequals(type, "24bit_sint"))
            {
                return SampleType::SIGNED24BIT;
            }
            else if (boost::iequals(type, "1bit"))
            {
                return SampleType::SINGLEBIT;
            }
            else if (boost::iequals(type, "complex"))
            {
                return SampleType::COMPLEX;
            }
            else if (boost::iequals(type, "blob"))
            {
                return SampleType::BLOB;
            }
            else if (boost::iequals(type, "reduced"))
            {
                return SampleType::REDUCED;
            }
            else if (boost::iequals(type, "raw"))
            {
                return SampleType::RAW;
            }

            // undefined type
            ODK_ASSERT_FAIL("Unsupported Sample Type");
            return SampleType::BLOB;
        }

        std::string stringifySampleType(SampleType type)
        {
            switch (type)
            {
            case SampleType::DOUBLE:
                return "double";

            case SampleType::SIGNED16BIT:
                return "16bit_sint";

            case SampleType::SIGNED24BIT:
                return "24bit_sint";

            case SampleType::SINGLEBIT:
                return "1bit";

            case SampleType::COMPLEX:
                return "complex";

            case SampleType::BLOB:
                return "blob";

            case SampleType::REDUCED:
                return "reduced";
            case SampleType::RAW:
                return "raw";

            default:
                ODK_ASSERT_FAIL("Unsupported Sample Type");
                return{};
            }
        }
    }

    Scaling::Scaling()
        : m_type(ScalingType::LINEAR)
        , m_factor(1)
        , m_offset(0)
    {
    }

    Scaling::Scaling(ScalingType type)
        : m_type(type)
        , m_factor(1)
        , m_offset(0)
    {
    }

    Scaling::Scaling(ScalingType type, double factor, double offset)
        : m_type(type)
        , m_factor(factor)
        , m_offset(offset)
    {      
    }

    bool Scaling::operator==(const Scaling& other) const
    {
        return m_type == other.m_type &&
               m_factor == other.m_factor &&
               m_offset == other.m_offset;
    }

    bool Scaling::operator!=(const Scaling& other) const
    {
        return !(*this == other);
    }

    ChannelDescriptor::ChannelDescriptor() noexcept
        : m_channel_id()
        , m_stride()
        , m_scaling()
        , m_ts_format()
        , m_size()
        , m_type()
        , m_dimension()
        , m_timestamp_position()
        , m_sample_size_position()
    {
    }

    bool ChannelDescriptor::operator==(const ChannelDescriptor& other) const
    {
        return m_channel_id == other.m_channel_id &&
               m_stride == other.m_stride &&
               m_scaling == other.m_scaling &&
               m_ts_format == other.m_ts_format &&
               m_size == other.m_size &&
               m_type == other.m_type &&
               m_dimension == other.m_dimension &&
               m_timestamp_position == other.m_timestamp_position &&
               m_sample_size_position == other.m_sample_size_position;
    }

    bool ChannelDescriptor::operator!=(const ChannelDescriptor& other) const
    {
        return !(*this == other);
    }

    StreamDescriptor::StreamDescriptor() noexcept
        : m_stream_id()
        , m_channel_descriptors()
    {
    }

    bool StreamDescriptor::operator==(const StreamDescriptor& other) const
    {
        return m_stream_id == other.m_stream_id &&
               m_channel_descriptors == other.m_channel_descriptors;
    }

    bool StreamDescriptor::operator!=(const StreamDescriptor& other) const
    {
        return !(*this == other);
    }


    DataSetDescriptor::DataSetDescriptor()
        : m_id()
        , m_stream_descriptors()
    {
    }

    bool DataSetDescriptor::parse(const char* xml_string)
    {
        pugi::xml_document doc;
        m_stream_descriptors.clear();
        auto status = doc.load_string(xml_string);
        if (status.status == pugi::status_ok)
        {
            try{

                auto block_desc_node = doc.document_element();

                m_id = boost::lexical_cast<std::uint64_t>(block_desc_node.attribute("data_set_key").value());

                for (auto scan_desc_node : block_desc_node.select_nodes("StreamDescriptor"))
                {
                    StreamDescriptor scan_desc;
                    auto a_scan_desc_node = scan_desc_node.node();
                    scan_desc.m_stream_id = boost::lexical_cast<std::uint64_t>(a_scan_desc_node.attribute("stream_id").value());

                    for (auto channel_desc_node : a_scan_desc_node.select_nodes("Channel"))
                    {
                        ChannelDescriptor channel_desc;
                        auto a_channel_desc_node = channel_desc_node.node();

                        channel_desc.m_channel_id = boost::lexical_cast<std::uint64_t>(a_channel_desc_node.attribute("channel_id").value());
                        channel_desc.m_dimension = boost::lexical_cast<std::uint32_t>(a_channel_desc_node.attribute("dimension").value());
                        channel_desc.m_size = boost::lexical_cast<std::uint32_t>(a_channel_desc_node.attribute("size").value());
                        channel_desc.m_stride = boost::lexical_cast<std::uint32_t>(a_channel_desc_node.attribute("stride").value());
                        auto type = boost::lexical_cast<std::string>(a_channel_desc_node.attribute("type").value());
                        channel_desc.m_type = parseSampleType(type);

                        auto scaling_children = a_channel_desc_node.select_nodes("Scaling/*");
                        for (auto child : scaling_children)
                        {
                            const auto n = child.node().name();
                            if (n && std::string(n) == "Linear")
                            {
                                Scaling linear_scaling;
                                linear_scaling.m_factor = boost::lexical_cast<double>(child.node().attribute("factor").value());
                                linear_scaling.m_offset = boost::lexical_cast<double>(child.node().attribute("offset").value());

                                channel_desc.m_scaling.push_back(linear_scaling);
                            }
                        }

                        auto timestamp_node = a_channel_desc_node.select_node("Timestamp");
                        if (timestamp_node)
                        {
                            channel_desc.m_timestamp_position = boost::lexical_cast<std::int32_t>(timestamp_node.node().attribute("position").value());
                        }

                        auto sample_size_position_node = a_channel_desc_node.select_node("Samplesize");
                        if (sample_size_position_node)
                        {
                            channel_desc.m_sample_size_position = boost::lexical_cast<std::int32_t>(sample_size_position_node.node().attribute("position").value());
                        }


                        scan_desc.m_channel_descriptors.push_back(channel_desc);
                    }

                    m_stream_descriptors.push_back(scan_desc);
                }
            }
            catch (const boost::bad_lexical_cast&)
            {
                return false;
            }
        }
        return true;
    }

    std::string DataSetDescriptor::generate() const
    {
        pugi::xml_document doc;
        auto data_set_desc_node = doc.append_child("DataSetDescriptor");

        data_set_desc_node.append_attribute("data_set_key").set_value(m_id);

        for (const auto& scan_descriptor : m_stream_descriptors)
        {
            auto scan_descriptor_node = data_set_desc_node.append_child("StreamDescriptor");
            scan_descriptor_node.append_attribute("stream_id").set_value(scan_descriptor.m_stream_id);

            for (const auto& channel_descriptor : scan_descriptor.m_channel_descriptors)
            {
                auto channel_descriptor_node = scan_descriptor_node.append_child("Channel");

                channel_descriptor_node.append_attribute("channel_id").set_value(channel_descriptor.m_channel_id);
                channel_descriptor_node.append_attribute("size").set_value(channel_descriptor.m_size);
                channel_descriptor_node.append_attribute("stride").set_value(channel_descriptor.m_stride);
                channel_descriptor_node.append_attribute("dimension").set_value(channel_descriptor.m_dimension);
                channel_descriptor_node.append_attribute("type").set_value(stringifySampleType(channel_descriptor.m_type).c_str());

                if (!channel_descriptor.m_scaling.empty())
                {
                    auto scaling_node = channel_descriptor_node.append_child("Scaling");
                    for (auto scaling_item : channel_descriptor.m_scaling)
                    {
                        auto scaling_type_node = scaling_node.append_child("Linear");
                        scaling_type_node.append_attribute("factor").set_value(scaling_item.m_factor);
                        scaling_type_node.append_attribute("offset").set_value(scaling_item.m_offset);
                    }
                }

                if (channel_descriptor.m_timestamp_position)
                {
                    auto timestamp_node = channel_descriptor_node.append_child("Timestamp");
                    timestamp_node.append_attribute("position").set_value(*channel_descriptor.m_timestamp_position);
                }

                if (channel_descriptor.m_sample_size_position)
                {
                    auto size_position_node = channel_descriptor_node.append_child("Samplesize");
                    size_position_node.append_attribute("position").set_value(*channel_descriptor.m_sample_size_position);
                }
            }
        }

        return xpugi::toXML(doc);
    }

    bool DataSetDescriptor::operator==(const DataSetDescriptor& other) const
    {
        return m_id == other.m_id &&
               m_stream_descriptors == other.m_stream_descriptors;
    }

    bool DataSetDescriptor::operator!=(const DataSetDescriptor& other) const
    {
        return !(*this == other);
    }

}
