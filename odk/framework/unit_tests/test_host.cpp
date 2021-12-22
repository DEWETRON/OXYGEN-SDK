// Copyright DEWETRON GmbH 2021
#include "test_host.h"
#include "values.h"

#include "odkuni_defines.h"
#include "odkapi_oxygen_queries.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/test/unit_test.hpp>

odk::IfValue* PLUGIN_API TestHost::createValue(odk::IfValue::Type type) const
{
    switch (type)
    {
    case odk::IfValue::Type::TYPE_BOOL:
        return new BooleanValue(false);
    case odk::IfValue::Type::TYPE_STRING:
        return new StringValue({});
    case odk::IfValue::Type::TYPE_XML:
        return new XmlValue({});
    default:
        BOOST_FAIL("Unsupported type");
        return nullptr;
    }
}

const odk::IfValue* PLUGIN_API TestHost::query(const char* context, const char* item, const odk::IfValue* param)
{
    ODK_UNUSED(param);
    if (boost::algorithm::equals(context, "#PluginHost"))
    {
        if (boost::algorithm::equals(item, odk::queries::PluginHost_Name))
        {
            return new StringValue("Oxygen");
        }
        if (boost::algorithm::equals(item, odk::queries::PluginHost_VersionString))
        {
            return new StringValue("5.6");
        }
    }
    BOOST_FAIL("Query not implemented");
    return nullptr;
}

const odk::IfValue* PLUGIN_API TestHost::queryXML(const char* context, const char* item, const char* xml, std::uint64_t xml_size)
{
    ODK_UNUSED(xml_size);
    auto xml_param = new XmlValue(xml);
    auto ret = query(context, item, xml_param);
    xml_param->release();
    return ret;
}
