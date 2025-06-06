// Copyright DEWETRON GmbH 2019
#pragma once

#define ODK_EXTENSION_FUNCTIONS //enable C++ integration

#include "odkapi_timebase_xml.h"
#include "odkapi_message_ids.h"
#include "odkbase_if_host.h"
#include "odkbase_basic_values.h"

#include "odkapi_timestamp_xml.h"

#include "odkuni_defines.h"

#include <array>
#include <cmath>
#include <cstring>
#include <limits>
#include <vector>

namespace odk
{
    ODK_NODISCARD bool isAnalysisModeActive(odk::IfHost* host);

    ODK_NODISCARD odk::Timestamp getMasterTimestamp(odk::IfHost* host);

    ODK_NODISCARD odk::AbsoluteTime getAcquisitionStartTime(odk::IfHost* host);

    ODK_NODISCARD odk::AbsoluteTime getMeasurementStartTime(odk::IfHost* host);

    /**
     * Sends a odk::host_msg::ADD_CONTIGUOUS_SAMPLES message to the host
     */
    void addSamples(odk::IfHost* host, std::uint32_t local_channel_id, std::uint64_t timestamp, const void* data, size_t data_size);

    /**
     * Sends a odk::host_msg::ADD_SAMPLE message to the host
     */
    void addSample(odk::IfHost* host, std::uint32_t local_channel_id, std::uint64_t timestamp, const void* data, size_t data_size);

    template <class T>
    inline void addSample(odk::IfHost* host, std::uint32_t local_channel_id, std::uint64_t timestamp, const T& data)
    {
        std::array<std::byte, sizeof(std::uint64_t) + sizeof(T)> sample;
        *reinterpret_cast<std::uint64_t*>(sample.data()) = timestamp;
        *reinterpret_cast<T*>(sample.data() + sizeof(std::uint64_t)) = data;

        host->messageSyncData(odk::host_msg::ADD_SAMPLE, local_channel_id, sample.data(), sample.size(), nullptr);
    }

    void updateChannelState(odk::IfHost* host, std::uint32_t local_channel_id, std::uint64_t timestamp);

    template <class T>
    bool parseXMLValue(const odk::IfValue* param, T& parser)
    {
        if (param && param->getType() == odk::IfValue::Type::TYPE_XML)
        {
            auto xml_value = static_cast<const odk::IfXMLValue*>(param);
            return parser.parse(xml_value->getValue());
        }
        return false;
    }

    std::uint64_t sendSyncXMLMessage(odk::IfHost* host, odk::MessageId msg_id, std::uint64_t key, const char* param_data, size_t param_size, const odk::IfValue** ret);

    /**
     * Convert from seconds to ticks
     *
     * @param time      sample time in seconds
     * @param frequency sample rate in Hz
     * @return          tick value
     */
    ODK_NODISCARD inline std::uint64_t convertTimeToTickAtOrAfter(double time, double frequency)
    {
        if (time == 0.0)
        {
            return 0;
        }
        return static_cast<std::uint64_t>(std::nextafter(std::nextafter(time, 0.0) * frequency, std::numeric_limits<double>::lowest())) + 1;
    }

    /**
     * Convert from tick values to time in seconds
     *
     * @param tick      sample position in ticks
     * @param frequency sample rate in Hz
     * @return          tick value converted to seconds
     */
    ODK_NODISCARD inline double convertTickToTime(std::uint64_t tick, double frequency)
    {
        double time = tick / frequency;
        if (static_cast<uint64_t>(time * frequency) < tick)
        {
            return std::nextafter(time, std::numeric_limits<double>::max());
        }
        return time; 
    }

    ODK_NODISCARD inline double convertTickToTime(const odk::Timestamp& ts)
    {
        return std::nextafter(ts.m_ticks / ts.m_frequency, std::numeric_limits<double>::max());
    }

    /**
     * Convert from seconds to ticks from a timebase with offset
     *
     * @param time      sample time in seconds
     * @param timebase  timebase with sample rate in Hz and offset
     * @return          tick value
     */
    ODK_NODISCARD inline std::uint64_t convertTimeToTickAtOrAfter(double time, const odk::Timebase& timebase)
    {
        if (time == 0.0)
        {
            return 0;
        }
        return convertTimeToTickAtOrAfter(time - timebase.m_offset, timebase.m_frequency);
    }

    /**
     * Convert from tick values to time in seconds from a timebase with offset
     *
     * @param tick      sample position in ticks
     * @param timebase  timebase with sample rate in Hz and offset
     * @return          tick value converted to seconds
     */
    ODK_NODISCARD inline double convertTickToTime(std::uint64_t tick, const odk::Timebase& timebase)
    {
        const auto offset = timebase.m_offset;
        return convertTickToTime(tick, timebase.m_frequency) + offset;
    }

    ODK_NODISCARD inline std::uint64_t convertTimestampToTick(const odk::Timestamp& timestamp, double target_frequency)
    {
        if (timestamp.m_frequency == target_frequency)
        {
            return timestamp.m_ticks;
        }
        double result = target_frequency * (timestamp.m_ticks / timestamp.m_frequency);
        return static_cast<std::uint64_t>(std::nextafter(result, std::numeric_limits<double>::max()));
    }

}
