// Copyright DEWETRON GmbH 2019

#include "odkfw_software_channel_plugin.h"

#include "odkfw_version_check.h"

#include "odkapi_utils.h"
#include "odkbase_message_return_value_holder.h"

namespace odk
{
namespace framework
{
    static const char* REQUIRED_OXYGEN_VERSION = "5.0.2";

    void SoftwareChannelPluginBase::addTranslation(const char* translation_xml)
    {
        odk::MessageReturnValueHolder<odk::IfErrorValue> ret_error;
        if (sendSyncXMLMessage(getHost(), odk::host_msg::ADD_TRANSLATION, 0, translation_xml, strlen(translation_xml) + 1, ret_error))
        {
            if (ret_error)
            {
                auto error_message = ret_error->getDescription();
            }
        }
    }

    void SoftwareChannelPluginBase::registerSoftwareChannel()
    {
        const auto telegram = getSoftwareChannelInfo();
        auto xml_msg = getHost()->template createValue<odk::IfXMLValue>();
        xml_msg->set(telegram.generate().c_str());
        getHost()->messageSync(odk::host_msg::SOFTWARE_CHANNEL_REGISTER, 0, xml_msg.get(), nullptr);
    }

    void SoftwareChannelPluginBase::unregisterSoftwareChannel()
    {
        auto channel_id = getHost()->template createValue<odk::IfStringValue>();
        const auto telegram = getSoftwareChannelInfo();
        channel_id->set(telegram.m_service_name.c_str());
        getHost()->messageSync(odk::host_msg::SOFTWARE_CHANNEL_UNREGISTER, 0, channel_id.get(), nullptr);
    }

    bool SoftwareChannelPluginBase::checkOxygenCompatibility()
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

