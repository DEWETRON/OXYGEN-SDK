// Copyright DEWETRON GmbH 2019
#pragma once

#define ODK_EXTENSION_FUNCTIONS //enable C++ integration

#include "odkbase_if_host.h"
#include "odkbase_basic_values.h"
#include "odkapi_message_ids.h"
#include "odkapi_oxygen_queries.h"

#include "odkapi_timestamp_xml.h"

#include "odkuni_assert.h"

#include <cstring>
#include <vector>

namespace odk
{

inline odk::Timestamp getMasterTimestamp(odk::IfHost* host)
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

inline void addSamples(odk::IfHost* host, std::uint32_t local_channel_id, std::uint64_t timestamp, const void* data, size_t data_size)
{
    std::vector<std::uint8_t> sample;
    sample.resize(8 + data_size);
    std::uint64_t* dst = reinterpret_cast<std::uint64_t*>(sample.data());
    *dst = timestamp;
    ++dst;
    std::memcpy(dst, data, data_size);

    host->messageSyncData(odk::host_msg::ADD_CONTIGUOUS_SAMPLES, local_channel_id, sample.data(), sample.size(), nullptr);
}

inline void addSample(odk::IfHost* host, std::uint32_t local_channel_id, std::uint64_t timestamp, const void* data, size_t data_size)
{
    std::vector<std::uint8_t> sample;
    sample.resize(8 + data_size);
    std::uint64_t* dst = reinterpret_cast<std::uint64_t*>(sample.data());
    *dst = timestamp;
    ++dst;
    std::memcpy(dst, data, data_size);

    host->messageSyncData(odk::host_msg::ADD_SAMPLE, local_channel_id, sample.data(), sample.size(), nullptr);
}

template <class T>
inline void addSample(odk::IfHost* host, std::uint32_t local_channel_id, std::uint64_t timestamp, const T& data)
{
    addSample(host, local_channel_id, timestamp, &data, sizeof(T));
}

inline void updateChannelState(odk::IfHost* host, std::uint32_t local_channel_id, std::uint64_t timestamp)
{
    auto timestamp_value = host->createValue<odk::IfUIntValue>();
    timestamp_value->set(timestamp);
    host->messageSync(odk::host_msg::UPDATE_CHANNEL_STATE, local_channel_id, timestamp_value.get(), nullptr);
}

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

inline std::uint64_t sendSyncXMLMessage(odk::IfHost* host, odk::MessageId msg_id, std::uint64_t key, const char* param_data, size_t param_size, const odk::IfValue** ret)
{
    auto xml_msg = host->createValue<odk::IfXMLValue>();
    xml_msg->set(param_data);
    assert(xml_msg->getLength() == param_size - 1);
    return host->messageSync(msg_id, key, xml_msg.get(), ret);
}

}
