// Copyright DEWETRON GmbH 2018

#pragma once

#include "odkbase_if_value.h"

namespace odk
{

    template <class T>
    class MessageReturnValueHolder
    {
    public:
        MessageReturnValueHolder()
            : m_value(nullptr)
        {
        }
        bool valid() const
        {
            return m_value && m_value->getType() == T::type_index;
        }
        operator bool() const
        {
            return valid();
        }
        const odk::IfValue** data()
        {
            return &m_value;
        }
        const T* operator->()
        {
            return static_cast<const T*>(m_value);
        }
        ~MessageReturnValueHolder()
        {
            if (m_value)
            {
                m_value->release();
            }
        }

    private:
        const odk::IfValue* m_value;
    };

}

