// Copyright DEWETRON GmbH 2021
#pragma once

#include "odkbase_if_host.h"
#include "values.h"

#include <boost/test/unit_test.hpp>

class TestHost : public odk::IfHost
{
public:
    TestHost() = default;

    odk::IfValue* PLUGIN_API createValue(odk::IfValue::Type type) const override
    {
        switch (type)
        {
        case odk::IfValue::Type::TYPE_STRING:
            return new StringValue({});
        case odk::IfValue::Type::TYPE_XML:
            return new XmlValue({});
        default:
            BOOST_FAIL("Unsupported type");
            return nullptr;
        }
    }

    const odk::IfValue* PLUGIN_API queryXML(const char* context, const char* item, const char* xml, std::uint64_t xml_size) override
    {
        auto xml_param = new XmlValue(xml);
        auto ret = query(context, item, xml_param);
        xml_param->release();
        return ret;
    }
};
