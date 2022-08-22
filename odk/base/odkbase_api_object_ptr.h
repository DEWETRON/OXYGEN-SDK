// Copyright DEWETRON GmbH 2017
#pragma once

#include "odkbase_if_api_object.h"
#include "odkuni_defines.h"
#include "odkuni_assert.h"

namespace odk
{

namespace detail
{
    template<class T>
    class ApiObjectPtr
    {
        typedef ApiObjectPtr this_type;
    public:
        typedef T element_type;

        ApiObjectPtr() noexcept = default;

        explicit ApiObjectPtr(T* p)
            : m_p(p)
        {
            //api objects are created with an initial ref count of 1,
            //which is adopted by ptr()
        }

        ApiObjectPtr(T* p, bool addRef)
            : m_p(p)
        {
            if (m_p && addRef)
            {
                intrusive_ptr_add_ref(m_p);
            }
        }

        ApiObjectPtr(ApiObjectPtr const& rhs)
            : m_p(rhs.m_p)
        {
            if (m_p != 0)
            {
                intrusive_ptr_add_ref(m_p);
            }
        }

        ~ApiObjectPtr()
        {
            if (m_p)
            {
                intrusive_ptr_release(m_p);
            }
        }

        template<class U>
        ApiObjectPtr& operator=(ApiObjectPtr<U> const& rhs)
        {
            this_type(rhs).swap(*this);
            return *this;
        }


        ApiObjectPtr& operator=(ApiObjectPtr const& rhs)
        {
            this_type(rhs).swap(*this);
            return *this;
        }

        ApiObjectPtr& operator=(T* rhs)
        {
            this_type(rhs).swap(*this);
            return *this;
        }

        T* get() const
        {
            return m_p;
        }

        T* detach()
        {
            T* ret = m_p;
            m_p = 0;
            return ret;
        }

        T& operator*() const
        {
            ODK_ASSERT(m_p != 0);
            return *m_p;
        }

        T* operator->() const
        {
            ODK_ASSERT(m_p != 0);
            return m_p;
        }

        typedef element_type* this_type::* unspecified_bool_type;

        operator unspecified_bool_type() const
        {
            return m_p == 0 ? 0 : &this_type::m_p;
        }

        bool operator! () const
        {
            return m_p == 0;
        }

        void swap(ApiObjectPtr& rhs)
        {
            T* tmp = m_p;
            m_p = rhs.m_p;
            rhs.m_p = tmp;
        }

        void reset()
        {
            this_type().swap(*this);
        }

        void reset(T* rhs)
        {
            this_type(rhs, true).swap(*this);
        }

        void reset(T* rhs, bool add_ref)
        {
            this_type(rhs, add_ref).swap(*this);
        }

    private:
        T* m_p = 0;
    };

    template<class T, class U> inline bool operator==(ApiObjectPtr<T> const& a, ApiObjectPtr<U> const& b)
    {
        return a.get() == b.get();
    }

    template<class T, class U> inline bool operator!=(ApiObjectPtr<T> const& a, ApiObjectPtr<U> const& b)
    {
        return a.get() != b.get();
    }

    template<class T, class U> inline bool operator==(ApiObjectPtr<T> const& a, U* b)
    {
        return a.get() == b;
    }

    template<class T, class U> inline bool operator!=(ApiObjectPtr<T> const& a, U* b)
    {
        return a.get() != b;
    }

    template<class T, class U> inline bool operator==(T* a, ApiObjectPtr<U> const& b)
    {
        return a == b.get();
    }

    template<class T, class U> inline bool operator!=(T* a, ApiObjectPtr<U> const& b)
    {
        return a != b.get();
    }

} // namespace detail

    /**
     * Returns a smart pointer to any object derived from IfApiObject.
     * The pointer adopts object p and handles reference counting automatically.
     * It therefore integrates well with common plugin get*-functions which return objects with pre-increased ref count.
     */
    template <class T>
    ODK_NODISCARD detail::ApiObjectPtr<T> ptr(T* p)
    {
        return detail::ApiObjectPtr<T>(p);
    }

    template <class T>
    ODK_NODISCARD detail::ApiObjectPtr<T> ptr_add_ref(T* p)
    {
        return detail::ApiObjectPtr<T>(p, true);
    }

    template <class T, class O>
    ODK_NODISCARD detail::ApiObjectPtr<T> api_ptr_cast(const detail::ApiObjectPtr<O>& ptr)
    {
        detail::ApiObjectPtr<T> ret;
        ret.reset(static_cast<T*>(ptr.get()));
        return ret;
    }

    template <class T, class O>
    ODK_NODISCARD detail::ApiObjectPtr<T> value_ptr_cast(const detail::ApiObjectPtr<O>& ptr)
    {
        detail::ApiObjectPtr<T> ret;
        if (ptr && ptr->getType() == T::type_index)
        {
            ret.reset(static_cast<T*>(ptr.get()));
        }
        return ret;
    }

    template <class T, class O>
    ODK_NODISCARD detail::ApiObjectPtr<const T> value_ptr_cast(const detail::ApiObjectPtr<const O>& ptr)
    {
        detail::ApiObjectPtr<const T> ret;
        if (ptr && ptr->getType() == T::type_index)
        {
            ret.reset(static_cast<const T*>(ptr.get()));
        }
        return ret;
    }
}
