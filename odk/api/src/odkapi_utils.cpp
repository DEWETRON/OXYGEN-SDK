// Copyright DEWETRON GmbH 2017

#include "odkapi_utils.h"

#include "odkapi_message_ids.h"
#include "odkapi_oxygen_queries.h"

#include "odkbase_if_host.h"

#include "odkuni_assert.h"

namespace odk
{
    bool isAnalysisModeActive(odk::IfHost* host)
    {
        auto analysis_active = host->getValue<odk::IfBooleanValue>(odk::queries::Oxygen, odk::queries::Oxygen_AnalysisModeActive);

        if (analysis_active)
        {
            return analysis_active->getValue();
        }

        return false;
    }

    odk::Timestamp getMasterTimestamp(odk::IfHost* host)
    {
        if (!isAnalysisModeActive(host))
        {
            auto master_timebase_xml = host->getValue<odk::IfXMLValue>(odk::queries::Oxygen, odk::queries::Oxygen_MasterTimebaseValue);

            if (!master_timebase_xml)
            {
                ODK_ASSERT_FAIL("Unable to retrieve master timebase value");
                return {};
            }

            odk::Timestamp master_timebase;
            master_timebase.parse(master_timebase_xml->getValue());
            return master_timebase;
        }
        return {};
    }

    odk::AbsoluteTime getAcquisitionStartTime(odk::IfHost* host)
    {
        auto xml_value = host->getValue<odk::IfXMLValue>(odk::queries::OxygenAcqStartTime, odk::queries::OxygenAcqStartTime_AbsXML);
        if (!xml_value)
        {
            ODK_ASSERT_FAIL("Unable to retrieve acquisition start time");
            return {};
        }
        odk::AbsoluteTime time;
        time.parse(xml_value->getValue());
        return time;
    }

    odk::AbsoluteTime getMeasurementStartTime(odk::IfHost* host)
    {
        auto xml_value = host->getValue<odk::IfXMLValue>(odk::queries::OxygenMeasStartTime, odk::queries::OxygenAcqStartTime_AbsXML);
        if (!xml_value)
        {
            ODK_ASSERT_FAIL("Unable to retrieve measurement start time");
            return {};
        }
        odk::AbsoluteTime time;
        time.parse(xml_value->getValue());
        return time;
    }

    void addSamples(odk::IfHost* host, std::uint32_t local_channel_id, std::uint64_t timestamp, const void* data, size_t data_size)
    {
        const std::uint8_t* timestamp_bytes = reinterpret_cast<const std::uint8_t*>(&timestamp);
        const std::uint8_t* data_bytes = reinterpret_cast<const std::uint8_t*>(data);

        std::vector<std::uint8_t> sample;
        sample.reserve(sizeof(std::uint64_t) + data_size); // reserve but do not fill with 0
        sample.insert(sample.end(), timestamp_bytes, timestamp_bytes + sizeof(std::uint64_t));
        sample.insert(sample.end(), data_bytes, data_bytes + data_size);

        host->messageSyncData(odk::host_msg::ADD_CONTIGUOUS_SAMPLES, local_channel_id, sample.data(), sample.size(), nullptr);
    }

    void addSample(odk::IfHost* host, std::uint32_t local_channel_id, std::uint64_t timestamp, const void* data, size_t data_size)
    {
        const std::uint8_t* timestamp_bytes = reinterpret_cast<const std::uint8_t*>(&timestamp);
        const std::uint8_t* data_bytes = reinterpret_cast<const std::uint8_t*>(data);

        std::vector<std::uint8_t> sample;
        sample.reserve(sizeof(std::uint64_t) + data_size); // reserve but do not fill with 0
        sample.insert(sample.end(), timestamp_bytes, timestamp_bytes + sizeof(std::uint64_t));
        sample.insert(sample.end(), data_bytes, data_bytes + data_size);

        host->messageSyncData(odk::host_msg::ADD_SAMPLE, local_channel_id, sample.data(), sample.size(), nullptr);
    }

    void updateChannelState(odk::IfHost* host, std::uint32_t local_channel_id, std::uint64_t timestamp)
    {
        auto timestamp_value = host->createValue<odk::IfUIntValue>();
        timestamp_value->set(timestamp);
        host->messageSync(odk::host_msg::UPDATE_CHANNEL_STATE, local_channel_id, timestamp_value.get(), nullptr);
    }

    std::uint64_t sendSyncXMLMessage(odk::IfHost* host, odk::MessageId msg_id, std::uint64_t key, const char* param_data, size_t param_size, const odk::IfValue** ret)
    {
        ODK_UNUSED(param_size);
        auto xml_msg = host->createValue<odk::IfXMLValue>();
        xml_msg->set(param_data);
        ODK_ASSERT_EQUAL(static_cast<std::size_t>(xml_msg->getLength()), param_size - 1);
        return host->messageSync(msg_id, key, xml_msg.get(), ret);
    }
}
