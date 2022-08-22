// Copyright DEWETRON GmbH 2017

#include "odkapi_timestamp_xml.h"

#include "odkuni_string_util.h"
#include "odkuni_xpugixml.h"

#include <cstring>

namespace odk
{
    Timestamp::Timestamp() noexcept
        : m_ticks(0)
        , m_frequency(0.0)
    {}

    Timestamp::Timestamp(std::uint64_t ticks, double frequency) noexcept
        : m_ticks(ticks)
        , m_frequency(frequency)
    {}

    bool Timestamp::timestampValid() const noexcept
    {
        return m_frequency > 0;
    }

    bool Timestamp::parse(const std::string_view& xml_string)
    {
        if (xml_string.empty())
            return false;

        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            auto node = doc.document_element();
            if (!node)
            {
                return false;
            }
            return parseTickFrequencyAttributes(node);
        }
        else
        {
            return false;
        }
    }

    std::string Timestamp::generate() const
    {
        pugi::xml_document doc;
        auto node = doc.append_child("Timestamp");
        writeTickFrequencyAttributes(node);
        return xpugi::toXML(doc);
    }

    void Timestamp::writeTickFrequencyAttributes(pugi::xml_node& node) const
    {
        node.append_attribute("ticks").set_value(m_ticks);
        node.append_attribute("frequency").set_value(m_frequency);
    }

    bool Timestamp::parseTickFrequencyAttributes(const pugi::xml_node& node)
    {
        m_ticks = 0;
        m_frequency = -1;

        try
        {
            auto ticks_attr = node.attribute("ticks");
            auto freq_attr = node.attribute("frequency");
            if (!ticks_attr || !freq_attr)
            {
                return false;
            }

            m_ticks = odk::from_string<std::uint64_t>(ticks_attr.value());
            m_frequency = odk::from_string<double>(freq_attr.value());
            return true;
        }
        catch (const std::logic_error&)
        {
            return false;
        }
    }

    AbsoluteTime::AbsoluteTime()
        : m_year(0)
        , m_month(0)
        , m_day(0)
        , m_hour(0)
        , m_minute(0)
        , m_second(0)
        , m_nanosecond(0)
        , m_nanoseconds_since_1970(0)
        , m_timezone_utc_offset_seconds(0)
        , m_timezone_std_offset_seconds(0)
        , m_timezone_dst_offset_seconds(0)
    {
    }

    bool AbsoluteTime::parse(const std::string_view& xml_string)
    {
        if (xml_string.empty())
            return false;

        pugi::xml_document doc;
        auto status = doc.load_buffer(xml_string.data(), xml_string.size(), pugi::parse_default, pugi::encoding_utf8);
        if (status.status == pugi::status_ok)
        {
            if (auto absolute_time_node = doc.child("AbsoluteTime"))
            {
                try
                {
                    m_year = odk::from_string<int>(absolute_time_node.attribute("year").value());
                    m_month = odk::from_string<int>(absolute_time_node.attribute("month").value());
                    m_day = odk::from_string<int>(absolute_time_node.attribute("day").value());
                    m_hour = odk::from_string<int>(absolute_time_node.attribute("hour").value());
                    m_minute = odk::from_string<int>(absolute_time_node.attribute("minute").value());
                    m_second = odk::from_string<int>(absolute_time_node.attribute("second").value());
                    m_nanosecond = odk::from_string<std::uint32_t>(absolute_time_node.attribute("nanosecond").value());

                    m_nanoseconds_since_1970 = odk::from_string<std::uint64_t>(absolute_time_node.attribute("nanoseconds_since_1970").value());

                    m_timezone_name = absolute_time_node.attribute("tz_name").value();
                    m_timezone_location = absolute_time_node.attribute("tz_location").value();
                    m_timezone_utc_offset_seconds = odk::from_string<int>(absolute_time_node.attribute("tz_utc_offset_seconds").value());
                    m_timezone_std_offset_seconds = odk::from_string<int>(absolute_time_node.attribute("tz_std_offset_seconds").value());
                    m_timezone_dst_offset_seconds = odk::from_string<int>(absolute_time_node.attribute("tz_dst_offset_seconds").value());

                    return true;
                }
                catch (const std::logic_error&)
                {
                }
            }
        }
        return false;
    }

    std::string AbsoluteTime::generate() const
    {
        pugi::xml_document doc;
        auto timestamp_node = doc.append_child("AbsoluteTime");
        timestamp_node.append_attribute("year")
            .set_value(odk::to_string(m_year).c_str());
        timestamp_node.append_attribute("month")
            .set_value(odk::to_string(m_month).c_str());
        timestamp_node.append_attribute("day")
            .set_value(odk::to_string(m_day).c_str());
        timestamp_node.append_attribute("hour")
            .set_value(odk::to_string(m_hour).c_str());
        timestamp_node.append_attribute("minute")
            .set_value(odk::to_string(m_minute).c_str());
        timestamp_node.append_attribute("second")
            .set_value(odk::to_string(m_second).c_str());
        timestamp_node.append_attribute("nanosecond")
            .set_value(odk::to_string(m_nanosecond).c_str());

        timestamp_node.append_attribute("nanoseconds_since_1970")
            .set_value(odk::to_string(m_nanoseconds_since_1970).c_str());

        timestamp_node.append_attribute("tz_name")
            .set_value(m_timezone_name.c_str());
        timestamp_node.append_attribute("tz_location")
            .set_value(m_timezone_location.c_str());
        timestamp_node.append_attribute("tz_utc_offset_seconds")
            .set_value(odk::to_string(m_timezone_utc_offset_seconds).c_str());
        timestamp_node.append_attribute("tz_std_offset_seconds")
            .set_value(odk::to_string(m_timezone_std_offset_seconds).c_str());
        timestamp_node.append_attribute("tz_dst_offset_seconds")
            .set_value(odk::to_string(m_timezone_dst_offset_seconds).c_str());

        auto xml_string = xpugi::toXML(doc);
        return xml_string;
    }

}
