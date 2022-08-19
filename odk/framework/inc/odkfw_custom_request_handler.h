// Copyright DEWETRON GmbH 2020
#pragma once

#include "odkfw_if_message_handler.h"
#include "odkapi_property_list_xml.h"
#include "odkbase_basic_values.h"

#include <functional>
#include <map>

namespace odk
{
namespace framework
{

    class CustomRequestHandler : public IfMessageHandler
    {
    public:

        //custom request the uses property lists as input and output
        using PropertyListFunction = std::uint64_t(const odk::PropertyList& params, odk::PropertyList& returns);

        //custom request the uses a property list as input and return an XML document
        using PropertyListToXMLFunction = std::uint64_t(const odk::PropertyList& params, odk::IfXMLValue& return_xml);

        using GenericHandlerFunction = std::uint64_t(odk::IfHost* host, const odk::IfValue* param, const odk::IfValue** ret);

        void registerFunction(std::uint16_t id, const char* name, std::function<PropertyListFunction> func);
        void registerFunction(std::uint16_t id, const char* name, std::function<PropertyListToXMLFunction> func);
        void registerFunction(std::uint16_t id, const char* name, std::function<GenericHandlerFunction> func);

    private:
        std::uint64_t pluginMessage(
            odk::PluginMessageId id,
            std::uint64_t key,
            const odk::IfValue* param,
            const odk::IfValue** ret) final;

        void setHost(odk::IfHost* host) final;

        odk::IfHost* m_host = nullptr;
        std::map<std::uint16_t, std::function<GenericHandlerFunction>> m_functions;
    };

}
}
