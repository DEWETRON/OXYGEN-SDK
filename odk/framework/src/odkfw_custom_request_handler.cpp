// Copyright DEWETRON GmbH 2020
#include "odkfw_custom_request_handler.h"

#define ODK_EXTENSION_FUNCTIONS
#include "odkfw_property_list_utils.h"
#include "odkbase_if_host.h"

namespace odk
{
namespace framework
{

    std::uint64_t CustomRequestHandler::pluginMessage(odk::PluginMessageId id, std::uint64_t key, const odk::IfValue* param, const odk::IfValue** ret)
    {
        if (id == odk::plugin_msg::CUSTOM_QML_REQUEST)
        {
            std::uint64_t ret_code = std::numeric_limits<uint64_t>::max();
            //limit to range that is safe for qml and reserve other uses
            if (key > 0 && key <= std::numeric_limits<std::int32_t>::max())
            {
                auto lid = static_cast<std::uint16_t>(key);
                auto it = m_functions.find(lid);
                if (it != m_functions.end())
                {
                    ret_code = it->second(m_host, param, ret);
                }
            }
            return ret_code;
        }
        return std::numeric_limits<uint64_t>::max();
    }

    void CustomRequestHandler::setHost(odk::IfHost* host)
    {
        m_host = host;
    }

    void CustomRequestHandler::registerFunction(std::uint16_t id, const char* name, std::function<PropertyListFunction> func)
    {
        auto f =
            [func](odk::IfHost* host, const odk::IfValue* param, const odk::IfValue** ret) -> std::uint64_t
            {
                odk::PropertyList param_list;
                if (param)
                {
                    if (!odk::framework::utils::convertToPropertyList(param, param_list))
                    {
                        return odk::error_codes::INVALID_INPUT_PARAMETER;
                    }
                }
                odk::PropertyList ret_list;
                auto ret_code = func(param_list, ret_list);
                if (!ret_list.empty())
                {
                    *ret = odk::framework::utils::convertToXMLValue(host, ret_list);
                }
                return ret_code;
            };
        registerFunction(id, name, f);
    }

    void CustomRequestHandler::registerFunction(std::uint16_t id, const char* name, std::function<PropertyListToXMLFunction> func)
    {
        auto f =
            [func](odk::IfHost* host, const odk::IfValue* param, const odk::IfValue** ret) -> std::uint64_t
            {
                odk::PropertyList param_list;
                if (param)
                {
                    if (!odk::framework::utils::convertToPropertyList(param, param_list))
                    {
                        return odk::error_codes::INVALID_INPUT_PARAMETER;
                    }
                }
                auto xml = host->createValue<odk::IfXMLValue>();
                auto ret_code = func(param_list, *xml);
                if (xml->getLength() > 0)
                {
                    *ret = xml.detach();
                }
                return ret_code;
            };
        registerFunction(id, name, f);
    }

    void CustomRequestHandler::registerFunction(std::uint16_t id, const char* name, std::function<GenericHandlerFunction> func)
    {
        ODK_UNUSED(name);
        m_functions[id] = func;
    }

}
}