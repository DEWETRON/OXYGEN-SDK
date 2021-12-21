// Copyright DEWETRON GmbH 2021
#pragma once
#include "odkbase_basic_values.h"
#include <atomic>

class StringValue : public odk::IfStringValue
{
public:
    StringValue(std::string value) : m_value(std::move(value)), m_ref(1) {}
    const char* PLUGIN_API getValue() const override { return m_value.c_str(); }
    int PLUGIN_API getLength() const override { return static_cast<int>(m_value.size()); }
    void PLUGIN_API set(const char* value) override { m_value = value; }

    void PLUGIN_API addRef() const override { ++m_ref; }
    void PLUGIN_API release() const override { if (--m_ref == 0) delete this; }
    Type PLUGIN_API getType() const override { return odk::IfStringValue::type_index; }
    const char* PLUGIN_API getDebugString() const override { return ""; }
protected:
    std::string m_value;
    mutable std::atomic<int> m_ref;
};

class XmlValue : public odk::IfXMLValue
{
public:
    XmlValue(std::string value) : m_value(std::move(value)), m_ref(1) {}
    const char* PLUGIN_API getValue() const override { return m_value.c_str(); }
    int PLUGIN_API getLength() const override { return static_cast<int>(m_value.size()); }
    void PLUGIN_API set(const char* value) override { m_value = value; }

    void PLUGIN_API addRef() const override { ++m_ref; }
    void PLUGIN_API release() const override { if (--m_ref == 0) delete this; }
    Type PLUGIN_API getType() const override { return odk::IfXMLValue::type_index; }
    const char* PLUGIN_API getDebugString() const override { return ""; }
protected:
    std::string m_value;
    mutable std::atomic<int> m_ref;
};
