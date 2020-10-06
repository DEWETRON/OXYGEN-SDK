// Copyright DEWETRON GmbH 2020
#include "odkfw_property_list_utils.h"

#define ODK_EXTENSION_FUNCTIONS
#include "odkbase_api_object_ptr.h"
#include "odkbase_basic_values.h"
#include "odkuni_xpugixml.h"
#include "odkbase_if_host.h"

namespace odk
{
namespace framework
{
namespace utils
{

    bool convertToPropertyList(const odk::IfValue* param, odk::PropertyList& properties)
    {
        if (param->getType() == odk::IfValue::Type::TYPE_XML)
        {
            auto param_xml = static_cast<const odk::IfXMLValue*>(param);

            pugi::xml_document doc;
            auto status = doc.load_string(param_xml->getValue());
            if (status.status != pugi::status_ok)
            {
                return false;
            }

            auto node = doc.document_element();
            if (!node)
            {
                return false;
            }

            if (!odk::strequal(node.name(), "PropertyList"))
            {
                return false;
            }

            if (!properties.readFrom(node, {}))
            {
                return false;
            }

            return true;
        }
        else
        {
            return false;
        }
    }

    odk::IfValue* convertToXMLValue(odk::IfHost* host, const odk::PropertyList& properties)
    {
        auto ret_xml = host->createValue<odk::IfXMLValue>();
        pugi::xml_document ret_doc;
        auto root = ret_doc.append_child("PropertyList");
        properties.appendTo(root);
        ret_xml->set(xpugi::toXML(ret_doc).c_str());
        return ret_xml.detach();
    }

}
}
}

