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
        operator const odk::IfValue**()
        {
            return &m_value;
        }
        bool valid()
        {
            return m_value && m_value->getType() == T::type_index;
        }
        operator bool() const
        {
            return valid;
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

