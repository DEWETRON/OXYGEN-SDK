// Copyright DEWETRON GmbH 2021
#pragma once
#include "odkbase_basic_values.h"
#include <atomic>

template<typename I>
class ValueBase : public I
{
public:
    ValueBase() : m_ref(1) {}
    virtual ~ValueBase() = default;
    void PLUGIN_API addRef() const final
    {
         m_ref.fetch_add(1, std::memory_order_relaxed);
    }
    void PLUGIN_API release() const final
    {
        if (m_ref.fetch_sub(1, std::memory_order_release) == 1)
        {
            std::atomic_thread_fence(std::memory_order_acquire);
            delete this;
        }
    }
    odk::IfValue::Type PLUGIN_API getType() const final { return I::type_index; }
    const char* PLUGIN_API getDebugString() const override { return ""; }
private:
    mutable std::atomic<int> m_ref;
};

class BooleanValue : public ValueBase<odk::IfBooleanValue>
{
public:
    BooleanValue(bool value) : m_value(value) {}
    bool PLUGIN_API getValue() const final { return m_value; }
    void PLUGIN_API set(bool value) final { m_value = value; }
protected:
    bool m_value;
};

class StringValue : public ValueBase<odk::IfStringValue>
{
public:
    StringValue(std::string value) : m_value(std::move(value)) {}
    const char* PLUGIN_API getValue() const final { return m_value.c_str(); }
    int PLUGIN_API getLength() const final { return static_cast<int>(m_value.size()); }
    void PLUGIN_API set(const char* value) final { m_value = value; }
protected:
    std::string m_value;
};

class XmlValue : public ValueBase<odk::IfXMLValue>
{
public:
    XmlValue(std::string value) : m_value(std::move(value)) {}
    const char* PLUGIN_API getValue() const final { return m_value.c_str(); }
    int PLUGIN_API getLength() const final { return static_cast<int>(m_value.size()); }
    void PLUGIN_API set(const char* value) final { m_value = value; }
protected:
    std::string m_value;
};
