#include "odkfw_plugin_base.h"
#include "odkbase_basic_values.h"

std::uint64_t PLUGIN_API odk::framework::PluginBase::pluginMessage(odk::PluginMessageId id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret)
{
    std::uint64_t ret_code = 0;
    if (handleMessage(id, key, param, ret, ret_code))
    {
        return ret_code;
    }

    for (auto& handler : m_message_handlers)
    {
        if (handler->pluginMessage(id, key, param, ret) == odk::error_codes::OK)
        {
            return odk::error_codes::OK;
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
    return -1;
}
