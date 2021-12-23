// Copyright DEWETRON GmbH 2017

#include "odkapi_block_descriptor_xml.h"
#include "odkfw_block_iterator.h"

#include <boost/test/unit_test.hpp>

using namespace odk::framework;

BOOST_AUTO_TEST_SUITE(block_iterator)

BOOST_AUTO_TEST_CASE(null_block_iterator_test)
{
    BlockIterator it;
    BOOST_CHECK_EQUAL(it.data(), nullptr);
    BOOST_CHECK_EQUAL(it.timestamp(), 0);

    ++it;
    BOOST_CHECK_EQUAL(it.data(), nullptr);
    BOOST_CHECK_EQUAL(it.timestamp(), 1);
}

BOOST_AUTO_TEST_CASE(timeless_block_iterator_test)
{
    const float data[] = {3.1415f, 2.718f, 1.618f};

    BlockIterator it(data, sizeof(float), 123);
    BOOST_CHECK_EQUAL(it.data(), data);
    BOOST_CHECK_EQUAL(it.timestamp(), 123);

    ++it;
    BOOST_CHECK_EQUAL(it.data(), data + 1);
    BOOST_CHECK_EQUAL(it.timestamp(), 124);

    --it;
    BOOST_CHECK_EQUAL(it.data(), data);
    BOOST_CHECK_EQUAL(it.timestamp(), 123);
}

BOOST_AUTO_TEST_CASE(timepointer_block_iterator_test)
{
    const double data[] = {3.1415, 2.718, 1.618};
    const std::uint64_t timestamps[] = {1, 2, 3};

    BlockIterator it(data, sizeof(double), timestamps, sizeof(std::uint64_t));
    auto it0 = it;
    BOOST_CHECK_EQUAL(it.data(), data);
    BOOST_CHECK_EQUAL(it.timestamp(), 1);

    ++it;
    auto it1 = it;
    BOOST_CHECK_EQUAL(it.data(), data + 1);
    BOOST_CHECK_EQUAL(it.timestamp(), 2);

    BOOST_CHECK(it == it1);
    BOOST_CHECK_EQUAL(it.distanceTo(it1), 0);
    BOOST_CHECK_EQUAL(it0.distanceTo(it1), 1);

    --it;
    BOOST_CHECK_EQUAL(it.data(), data);
    BOOST_CHECK_EQUAL(it.timestamp(), 1);

    BOOST_CHECK(it == it0);
    BOOST_CHECK(it != it1);
    BOOST_CHECK_EQUAL(it.distanceTo(it1), 1);
}

BOOST_AUTO_TEST_SUITE_END()
