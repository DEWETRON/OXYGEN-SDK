// Copyright DEWETRON GmbH 2017

#include "odkapi_data_set_descriptor_xml.h"
#include "odkapi_xml_builder.h"

#include "odkuni_assert.h"
#include "odkuni_string_util.h"
#include "odkuni_xpugixml.h"

#include <algorithm>
#include <cctype>
#include <sstream>

namespace odk
{

    namespace
    {
        SampleType parseSampleType(const std::string& type)
        {
            std::string type_lower = type;
            std::transform(type_lower.begin(), type_lower.end(), type_lower.begin(),
                [](std::string::value_type c)
                { // only safe/sufficient since we want to compare for equality to 7-bit ASCII strings
                    auto r = c;
                    if (c >= 'A' && c <= 'Z')
                    {
                        r = c - ('Z' - 'z');
                    }
                    return r;
                }
            );
            if (type_lower == "double")
            {
                return SampleType::DOUBLE;
            }
            else if (type_lower == "16bit_sint")
            {
                return SampleType::SIGNED16BIT;
            }
            else if (type_lower == "24bit_sint")
            {
                return SampleType::SIGNED24BIT;
            }
            else if (type_lower == "1bit")
            {
                return SampleType::SINGLEBIT;
            }
            else if (type_lower == "complex")
            {
                return SampleType::COMPLEX;
            }
            else if (type_lower == "blob")
            {
                return SampleType::BLOB;
            }
            else if (type_lower == "reduced")
            {
                return SampleType::REDUCED;
            }
            else if (type_lower == "raw")
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

    Scaling::Scaling() noexcept
        : m_type(ScalingType::LINEAR)
        , m_factor(1)
        , m_offset(0)
    {
    }

    Scaling::Scaling(ScalingType type) noexcept
        : m_type(type)
        , m_factor(1)
        , m_offset(0)
    {
    }

    Scaling::Scaling(ScalingType type, double factor, double offset) noexcept
        : m_type(type)
        , m_factor(factor)
        , m_offset(offset)
    {
    }

    bool Scaling::operator==(const Scaling& other) const noexcept
    {
        return m_type == other.m_type &&
               m_factor == other.m_factor &&
               m_offset == other.m_offset;
    }

    bool Scaling::operator!=(const Scaling& other) const noexcept
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


    DataSetDescriptor::DataSetDescriptor() noexcept
        : m_id()
        , m_stream_descriptors()
        , m_all_channels_registered(false)
    {
    }

    bool DataSetDescriptor::parse(const std::string_view& xml_string)
    {
        pugi::xml_document doc;
        m_stream_descriptors.clear();
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            try{

                auto block_desc_node = doc.document_element();

                m_id = odk::from_string<std::uint64_t>(block_desc_node.attribute("data_set_key").value());
                m_all_channels_registered = block_desc_node.attribute("all_channels_registered").as_bool();

                for (auto scan_desc_node : block_desc_node.children("StreamDescriptor"))
                {
                    StreamDescriptor scan_desc;
                    scan_desc.m_stream_id = odk::from_string<std::uint64_t>(scan_desc_node.attribute("stream_id").value());

                    for (auto channel_desc_node : scan_desc_node.children("Channel"))
                    {
                        ChannelDescriptor channel_desc;

                        channel_desc.m_channel_id = odk::from_string<std::uint64_t>(channel_desc_node.attribute("channel_id").value());
                        channel_desc.m_dimension = odk::from_string<std::uint32_t>(channel_desc_node.attribute("dimension").value());
                        channel_desc.m_size = odk::from_string<std::uint32_t>(channel_desc_node.attribute("size").value());
                        channel_desc.m_stride = odk::from_string<std::uint32_t>(channel_desc_node.attribute("stride").value());
                        auto type = std::string(channel_desc_node.attribute("type").value());
                        channel_desc.m_type = parseSampleType(type);

                        auto scaling_children = channel_desc_node.select_nodes("Scaling/*");
                        for (auto child : scaling_children)
                        {
                            const auto n = child.node().name();
                            if (n && std::string(n) == "Linear")
                            {
                                Scaling linear_scaling;
                                linear_scaling.m_factor = odk::from_string<double>(child.node().attribute("factor").value());
                                linear_scaling.m_offset = odk::from_string<double>(child.node().attribute("offset").value());

                                channel_desc.m_scaling.push_back(linear_scaling);
                            }
                        }

                        auto timestamp_node = channel_desc_node.child("Timestamp");
                        if (timestamp_node)
                        {
                            channel_desc.m_timestamp_position = odk::from_string<std::int32_t>(timestamp_node.attribute("position").value());
                        }

                        auto sample_size_position_node = channel_desc_node.child("Samplesize");
                        if (sample_size_position_node)
                        {
                            channel_desc.m_sample_size_position = odk::from_string<std::int32_t>(sample_size_position_node.attribute("position").value());
                        }


                        scan_desc.m_channel_descriptors.push_back(channel_desc);
                    }

                    m_stream_descriptors.push_back(scan_desc);
                }
            }
            catch (const std::logic_error&)
            {
                return false;
            }
        }
        return true;
    }

    std::string DataSetDescriptor::generate() const
    {
        std::ostringstream stream;

        {
            using odk::xml_builder::Attribute;
            odk::xml_builder::Document doc(stream);
            auto data_set_desc_node = doc.append_child("DataSetDescriptor",
                Attribute("data_set_key", m_id),
                Attribute("all_channels_registered", m_all_channels_registered));

            for (const auto& scan_descriptor : m_stream_descriptors)
            {
                auto scan_descriptor_node = data_set_desc_node.append_child("StreamDescriptor",
                    Attribute("stream_id", scan_descriptor.m_stream_id));

                for (const auto& channel_descriptor : scan_descriptor.m_channel_descriptors)
                {
                    auto channel_descriptor_node = scan_descriptor_node.append_child("Channel");

                    channel_descriptor_node.append_attribute("channel_id", channel_descriptor.m_channel_id);
                    channel_descriptor_node.append_attribute("size", channel_descriptor.m_size);
                    channel_descriptor_node.append_attribute("stride", channel_descriptor.m_stride);
                    channel_descriptor_node.append_attribute("dimension", channel_descriptor.m_dimension);
                    channel_descriptor_node.append_attribute("type", stringifySampleType(channel_descriptor.m_type));

                    if (!channel_descriptor.m_scaling.empty())
                    {
                        auto scaling_node = channel_descriptor_node.append_child("Scaling");
                        for (const auto& scaling_item : channel_descriptor.m_scaling)
                        {
                            scaling_node.append_child("Linear",
                                Attribute("factor", scaling_item.m_factor),
                                Attribute("offset", scaling_item.m_offset));
                        }
                    }

                    if (channel_descriptor.m_timestamp_position)
                    {
                        channel_descriptor_node.append_child("Timestamp",
                            Attribute("position", *channel_descriptor.m_timestamp_position));
                    }

                    if (channel_descriptor.m_sample_size_position)
                    {
                        channel_descriptor_node.append_child("Samplesize",
                            Attribute("position", *channel_descriptor.m_sample_size_position));
                    }
                }
            }
        }

        return stream.str();
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
