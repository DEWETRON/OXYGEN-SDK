// Copyright DEWETRON GmbH 2021
#pragma once

#include "odkbase_if_host.h"

class TestHost : public odk::IfHost
{
public:
    TestHost() = default;

    odk::IfValue* PLUGIN_API createValue(odk::IfValue::Type type) const override;

    const odk::IfValue* PLUGIN_API query(const char* context, const char* item, const odk::IfValue* param) override;

    const odk::IfValue* PLUGIN_API queryXML(const char* context, const char* item, const char* xml, std::uint64_t xml_size) override;
};
