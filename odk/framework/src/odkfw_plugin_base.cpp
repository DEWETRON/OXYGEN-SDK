#include "odkfw_plugin_base.h"

#include "odkapi_utils.h"
#include "odkbase_basic_values.h"
#include "odkbase_if_host.h"
#include "odkbase_message_return_value_holder.h"

std::uint64_t PLUGIN_API odk::framework::PluginBase::pluginMessage(odk::PluginMessageId id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret)
{
    std::uint64_t ret_code = 0;
    if (handleMessage(id, key, param, ret, ret_code))
    {
        return ret_code;
    }

    for (auto& handler : m_message_handlers)
    {
        ret_code = handler->pluginMessage(id, key, param, ret);
        if (ret_code != odk::error_codes::NOT_IMPLEMENTED)
        {
            return ret_code;
        }
    }

    switch (id)
    {
        case odk::plugin_msg::INIT:
        {
            std::string error_message;
            auto error_code = init(error_message);
            if (error_code != odk::error_codes::OK && ret && m_host && error_message.empty())
            {
                auto err_val = m_host->createValue<odk::IfErrorValue>();
                if (err_val)
                {
                    err_val->set(error_code, error_message.c_str());
                    *ret = err_val.detach();
                }
            }
            return error_code;
        }
        case odk::plugin_msg::DEINIT:
            return deinit() ? odk::error_codes::OK : odk::error_codes::INTERNAL_ERROR;
    }
    return std::numeric_limits<uint64_t>::max();
}

bool odk::framework::PluginBase::addTranslation(const char* translation_xml)
{
    odk::MessageReturnValueHolder<odk::IfErrorValue> ret_error;
    if (sendSyncXMLMessage(getHost(), odk::host_msg::ADD_TRANSLATION, 0, translation_xml, strlen(translation_xml) + 1, ret_error.data()))
    {
        if (ret_error)
        {
            auto error_message = ret_error->getDescription();
            ODK_UNUSED(error_message);
        }
        return false;
    }
    return true;
}

bool odk::framework::PluginBase::addQtResources(const void* rcc_data, std::uint64_t rcc_size)
{
    odk::MessageReturnValueHolder<odk::IfErrorValue> ret_error;

    if (getHost()->messageSyncData(odk::host_msg::ADD_RESOURCES, 0, rcc_data, rcc_size) != odk::error_codes::OK)
    {
        if (ret_error)
        {
            auto error_message = ret_error->getDescription();
            ODK_UNUSED(error_message);
        }
        return false;
    }
    return true;
}


