// Copyright DEWETRON GmbH 2020

#include "odkfw_export_plugin.h"

#include "odkfw_version_check.h"

#include "odkapi_utils.h"
#include "odkbase_message_return_value_holder.h"

namespace odk
{
namespace framework
{
    static const char* REQUIRED_OXYGEN_VERSION = "5.3";

    void ExportPluginBase::registerExport()
    {
        const auto telegram = getExportInfo();
        auto xml_msg = getHost()->template createValue<odk::IfXMLValue>();
        xml_msg->set(telegram.generate().c_str());
        getHost()->messageSync(odk::host_msg::EXPORT_REGISTER, 0, xml_msg.get(), nullptr);
    }

    void ExportPluginBase::unregisterExport()
    {
        auto format_id = getHost()->template createValue<odk::IfStringValue>();
        const auto telegram = getExportInfo();
        format_id->set(telegram.m_format_id.c_str());
        getHost()->messageSync(odk::host_msg::EXPORT_UNREGISTER, 0, format_id.get(), nullptr);
    }

    bool ExportPluginBase::checkOxygenCompatibility()
    {
        auto name = getHost()->getValue<odk::IfStringValue>(odk::queries::PluginHost, odk::queries::PluginHost_Name);
        auto version = getHost()->getValue<odk::IfStringValue>(odk::queries::PluginHost, odk::queries::PluginHost_VersionString);
        if (name && version)
        {
            if (std::string("Oxygen") == name->getValue())
            {
                if (compareVersionStrings(REQUIRED_OXYGEN_VERSION, version->getValue()) <= 0)
                {
                    return true;
                }
            }
        }
        return false;
    }
}
}

