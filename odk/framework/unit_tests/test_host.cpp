// Copyright DEWETRON GmbH 2021
#include "test_host.h"
#include "values.h"

#include "odkuni_defines.h"
#include "odkapi_oxygen_queries.h"
#include "odkapi_error_codes.h"

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

std::uint64_t PLUGIN_API TestHost::messageSync(odk::MessageId msg_id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret)
{
    ODK_UNUSED(msg_id);
    ODK_UNUSED(key);
    ODK_UNUSED(param);
    ODK_UNUSED(ret);
    BOOST_FAIL("Unexpected message");
    return odk::error_codes::NOT_IMPLEMENTED;
}

std::uint64_t PLUGIN_API TestHost::messageSyncData(odk::MessageId msg_id, std::uint64_t key, const void* param, std::uint64_t param_size, const odk::IfValue** ret)
{
    ODK_UNUSED(msg_id);
    ODK_UNUSED(key);
    ODK_UNUSED(param);
    ODK_UNUSED(param_size);
    ODK_UNUSED(ret);
    BOOST_FAIL("Unexpected message");
    return odk::error_codes::NOT_IMPLEMENTED;
}

std::uint64_t PLUGIN_API TestHost::messageAsync(odk::MessageId msg_id, std::uint64_t key, const odk::IfValue* param)
{
    ODK_UNUSED(msg_id);
    ODK_UNUSED(key);
    ODK_UNUSED(param);
    BOOST_FAIL("Unexpected message");
    return odk::error_codes::NOT_IMPLEMENTED;
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
    XmlValue xml_param(xml);
    auto ret = query(context, item, &xml_param);
    return ret;
}
