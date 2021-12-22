// Copyright DEWETRON GmbH 2021
#pragma once

#include "odkbase_if_host.h"

class TestHost : public odk::IfHost
{
public:
    TestHost() = default;

    odk::IfValue* PLUGIN_API createValue(odk::IfValue::Type type) const override;

    std::uint64_t PLUGIN_API messageSync(odk::MessageId msg_id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret) override;

    std::uint64_t PLUGIN_API messageSyncData(odk::MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const odk::IfValue** ret) override;

    std::uint64_t PLUGIN_API messageAsync(odk::MessageId msg_id, std::uint64_t key, const odk::IfValue* param) override;

    const odk::IfValue* PLUGIN_API query(const char* context, const char* item, const odk::IfValue* param) override;

    const odk::IfValue* PLUGIN_API queryXML(const char* context, const char* item, const char* xml, std::uint64_t xml_size) override;
};
