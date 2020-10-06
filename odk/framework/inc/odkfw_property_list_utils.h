// Copyright DEWETRON GmbH 2020
#pragma once

#include "odkbase_if_value.h"
#include "odkapi_property_list_xml.h"

namespace odk
{
    class IfHost;

namespace framework
{
namespace utils
{

    bool convertToPropertyList(const odk::IfValue* param, odk::PropertyList& properties);

    odk::IfValue* convertToXMLValue(odk::IfHost* host, const odk::PropertyList& properties);
}
}
}

