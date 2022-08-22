// Copyright DEWETRON GmbH 2022

#include "odkbase_api_object_ptr.h"

#include <boost/test/unit_test.hpp>

using namespace odk;

struct Dummy1 : public IfApiObject
{
    mutable int m_ref_count = 1;

    void PLUGIN_API addRef() const
    {
        BOOST_CHECK(m_ref_count > 0);
        ++m_ref_count;
    }
    void PLUGIN_API release() const
    {
        BOOST_CHECK(m_ref_count > 0);
        --m_ref_count;
    }

};

struct Dummy2 : public Dummy1
{
    void is2()
    {
    }
};

BOOST_AUTO_TEST_SUITE(object_ptr_tests)

BOOST_AUTO_TEST_CASE(basic_ref_counting)
{
    {
        Dummy1 o;
        BOOST_CHECK_EQUAL(o.m_ref_count, 1);
        auto p = ptr(&o);
        BOOST_CHECK_EQUAL(o.m_ref_count, 1);
        {
            auto p2 = p;
            BOOST_CHECK_EQUAL(o.m_ref_count, 2);
        }
        BOOST_CHECK_EQUAL(o.m_ref_count, 1);
        {
            auto p2 = p;
            BOOST_CHECK_EQUAL(o.m_ref_count, 2);
            p2.reset();
            BOOST_CHECK_EQUAL(o.m_ref_count, 1);
            p2.reset(&o);
            BOOST_CHECK_EQUAL(o.m_ref_count, 2);
        }
        p.reset();
        BOOST_CHECK_EQUAL(o.m_ref_count, 0);
    }
}

BOOST_AUTO_TEST_CASE(casting)
{
    {
        Dummy2 o2;
        auto p2 = ptr(&o2);
        auto p1 = api_ptr_cast<Dummy1>(p2);
        BOOST_CHECK(p1.get() != 0);
        BOOST_CHECK_EQUAL(o2.m_ref_count, 2);
    }

    {
        Dummy1 o1;
        auto p1 = ptr(&o1);
        auto p2 = api_ptr_cast<Dummy2>(p1);
        BOOST_CHECK(p2.get() != 0);
        BOOST_CHECK_EQUAL(o1.m_ref_count, 2);
    }
}
//value_ptr_cast
BOOST_AUTO_TEST_SUITE_END()
