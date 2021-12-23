// Copyright DEWETRON GmbH 2017

#include "odkfw_stream_iterator.h"
#include "odkapi_block_descriptor_xml.h"

#include <boost/test/unit_test.hpp>

using namespace odk::framework;

BOOST_AUTO_TEST_SUITE(stream_iterator)

template<class ValueType>
void addSyncDataRange(StreamIterator& it, const std::vector<ValueType>& data, std::uint64_t start_index)
{
    it.addRange(BlockIterator(data.data(), sizeof(ValueType), start_index),
                BlockIterator(data.data() + data.size(), sizeof(ValueType), start_index+data.size()));
}


template<class ValueType>
void addAsyncDataRange(StreamIterator& it, const std::vector<ValueType>& data, const std::vector<std::uint64_t>& timestamp_data)
{
    it.addRange(BlockIterator(data.data(), sizeof(ValueType), timestamp_data.data(), sizeof(std::uint64_t)),
                BlockIterator(data.data() + data.size(), sizeof(ValueType), timestamp_data.data() + timestamp_data.size(), sizeof(std::uint64_t), BlockIterator::no_value_read()));
}

template<class ValueType>
void addAsyncVariableDataRange(StreamIterator& it, const std::vector<ValueType>& data,
                               const std::vector<std::uint64_t>& timestamp_data,
                               const std::vector<std::uint32_t>& size_data)
{
    it.addRange(BlockIterator(data.data(), 0, timestamp_data.data(), 0, size_data.data(), 0),
                BlockIterator(data.data() + data.size(), 0, timestamp_data.data() + timestamp_data.size(), 0, size_data.data() + size_data.size(), 0, BlockIterator::no_value_read()));
}

BOOST_AUTO_TEST_CASE(empty_stream_iterator_test)
{
    StreamIterator it;
    BOOST_CHECK(!it.valid());
    BOOST_CHECK(it.data() == nullptr);
    BOOST_CHECK_EQUAL(it.timestamp(), 0);
    BOOST_CHECK_EQUAL(it.getTotalSampleCount(), 0);
}

BOOST_AUTO_TEST_CASE(single_block_stream_iterator_test)
{
    StreamIterator it;
    BOOST_CHECK(!it.valid());

    std::vector<double> data = { 2711, 618 };
    addSyncDataRange(it, data, 100);

    BOOST_CHECK(it.valid());
    BOOST_CHECK_EQUAL(it.value<double>(), data[0]);
    BOOST_CHECK_EQUAL(it.timestamp(), 100);
    BOOST_CHECK_EQUAL(it.getTotalSampleCount(), 2);

    ++it;
    BOOST_CHECK(it.valid());
    BOOST_CHECK_EQUAL(it.value<double>(), data[1]);
    BOOST_CHECK_EQUAL(it.timestamp(), 101);
    BOOST_CHECK_EQUAL(it.getTotalSampleCount(), 2);

    ++it;
    BOOST_CHECK(!it.valid());
    BOOST_CHECK(it.data() == nullptr);
    BOOST_CHECK_EQUAL(it.timestamp(), 0);
    BOOST_CHECK_EQUAL(it.getTotalSampleCount(), 2);
}

BOOST_AUTO_TEST_CASE(double_block_stream_iterator_test)
{
    StreamIterator it;
    BOOST_CHECK(!it.valid());

    std::vector<double> data = { 2711, 618 };
    std::vector<double> data2 = { 15, 17 };

    addSyncDataRange(it, data, 100);
    addSyncDataRange(it, data2, 102);
    BOOST_CHECK_EQUAL(it.getTotalSampleCount(), 4);

    BOOST_CHECK(it.valid());
    BOOST_CHECK_EQUAL(it.value<double>(), data[0]);
    BOOST_CHECK_EQUAL(it.timestamp(), 100);

    ++it;
    BOOST_CHECK(it.valid());
    BOOST_CHECK_EQUAL(it.value<double>(), data[1]);
    BOOST_CHECK_EQUAL(it.timestamp(), 101);

    ++it;
    BOOST_CHECK(it.valid());
    BOOST_CHECK_EQUAL(it.value<double>(), data2[0]);
    BOOST_CHECK_EQUAL(it.timestamp(), 102);

    ++it;
    BOOST_CHECK(it.valid());
    BOOST_CHECK_EQUAL(it.value<double>(), data2[1]);
    BOOST_CHECK_EQUAL(it.timestamp(), 103);

    ++it;
    BOOST_CHECK(!it.valid());
    BOOST_CHECK(it.data() == nullptr);
    BOOST_CHECK_EQUAL(it.timestamp(), 0);
}

BOOST_AUTO_TEST_CASE(bidir_block_stream_iterator_test)
{
    StreamIterator it;
    BOOST_CHECK(!it.valid());

    std::vector<double> data = { 2711, 618 };
    std::vector<double> data2 = { 15, 17 };

    addSyncDataRange(it, data, 100);
    addSyncDataRange(it, data2, 102);

    ++it;
    ++it;
    BOOST_CHECK(it.valid());
    BOOST_CHECK_EQUAL(it.value<double>(), data2[0]);
    BOOST_CHECK_EQUAL(it.timestamp(), 102);

    --it;
    BOOST_CHECK(it.valid());
    BOOST_CHECK_EQUAL(it.value<double>(), data[1]);
    BOOST_CHECK_EQUAL(it.timestamp(), 101);

    --it;
    BOOST_CHECK(it.valid());
    BOOST_CHECK_EQUAL(it.value<double>(), data[0]);
    BOOST_CHECK_EQUAL(it.timestamp(), 100);

    --it;
    BOOST_CHECK(!it.valid());
    BOOST_CHECK(it.data() == nullptr);
    BOOST_CHECK_EQUAL(it.timestamp(), 0);
}


BOOST_AUTO_TEST_CASE(block_stream_iterator_empty_range_test)
{
    StreamIterator it;
    it.setSkipGaps(false);
    BOOST_CHECK(!it.valid());

    std::vector<double> data = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    addSyncDataRange(it, data, 10);
    it.addRange(BlockIterator(20), BlockIterator(30));
    addSyncDataRange(it, data, 30);
    it.addRange(BlockIterator(40), BlockIterator(50));
    addSyncDataRange(it, data, 50);

    for(int i = 10; i <= 59; ++i)
    {
        BOOST_CHECK(it.valid());
        BOOST_CHECK_EQUAL(it.timestamp(), i);
        ++it;
    }

    BOOST_CHECK(!it.valid());
}

BOOST_AUTO_TEST_CASE(block_stream_iterator_gap_test)
{
    StreamIterator it;
    it.setSkipGaps(false);
    BOOST_CHECK(!it.valid());

    std::vector<double> data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    addSyncDataRange(it, data, 10);
    addSyncDataRange(it, data, 30);

    it.addRange(BlockIterator(45), BlockIterator(47));

    for(int i = 10; i <= 19; ++i)
    {
        BOOST_CHECK_EQUAL(it.value<double>(), i % 10);
        BOOST_CHECK(it.valid());
        BOOST_CHECK_EQUAL(it.timestamp(), i);
        ++it;
    }

    for(int i = 30; i <= 39; ++i)
    {
        BOOST_CHECK_EQUAL(it.value<double>(), i % 10);
        BOOST_CHECK(it.valid());
        BOOST_CHECK_EQUAL(it.timestamp(), i);
        ++it;
    }

    for(int i = 45; i <= 46; ++i)
    {
        BOOST_CHECK_EQUAL(it.data(), nullptr);
        BOOST_CHECK(it.valid());
        BOOST_CHECK_EQUAL(it.timestamp(), i);
        ++it;
    }

    BOOST_CHECK(!it.valid());
}


BOOST_AUTO_TEST_CASE(stream_iterator_async_test)
{
    StreamIterator it;
    BOOST_CHECK(!it.valid());

    std::vector<double> data = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    std::vector<std::uint64_t> timestamps = { 0, 10, 20, 30, 40, 50, 60, 70, 80, 90 };
    addAsyncDataRange(it, data, timestamps);

    for(int i = 0; i <= 9; ++i)
    {
        BOOST_CHECK_EQUAL(it.value<double>(), i);
        BOOST_CHECK(it.valid());
        BOOST_CHECK_EQUAL(it.timestamp(), i*10);
        ++it;
    }

    BOOST_CHECK(!it.valid());
}

BOOST_AUTO_TEST_CASE(stream_iterator_async_same_timestamp_test)
{
    StreamIterator it;
    BOOST_CHECK(!it.valid());

    std::vector<double> data = { 0, 1, 2, 3, 4, 5};
    std::vector<std::uint64_t> timestamps = { 0, 1, 1, 2, 3, 3};
    std::vector<std::uint64_t> timestamps2 = { 4, 4, 5, 6, 7, 8};
    addAsyncDataRange(it, data, timestamps);
    addAsyncDataRange(it, data, timestamps2);

    for(int i = 0; i <= 5; ++i)
    {
        BOOST_CHECK_EQUAL(it.value<double>(), data[i]);
        BOOST_CHECK(it.valid());
        BOOST_CHECK_EQUAL(it.timestamp(), timestamps[i]);
        ++it;
    }

    for(int i = 0; i <= 5; ++i)
    {
        BOOST_CHECK_EQUAL(it.value<double>(), data[i]);
        BOOST_CHECK(it.valid());
        BOOST_CHECK_EQUAL(it.timestamp(), timestamps2[i]);
        ++it;
    }

    BOOST_CHECK(!it.valid());
}

BOOST_AUTO_TEST_CASE(stream_iterator_async_variable_size_test)
{
    StreamIterator it;
    BOOST_CHECK(!it.valid());

    std::vector<double> data = { 0, 1, 991, 2, 992, 993, 3, 4};
    std::vector<std::uint64_t> timestamps = { 0, 10, 991, 20, 992, 993, 30, 40};
    std::vector<std::uint32_t> sizes = { 8, 999, 16, 998, 997, 996, 24, 995, 994, 993, 992, 991, 8, 990, 8, 989};
    addAsyncVariableDataRange(it, data, timestamps, sizes);

    for(int i = 0; i <= 4; ++i)
    {
        BOOST_CHECK_EQUAL(it.value<double>(), i);
        BOOST_CHECK(it.valid());
        BOOST_CHECK_EQUAL(it.timestamp(), i*10);
        ++it;
    }

    BOOST_CHECK(!it.valid());
}

BOOST_AUTO_TEST_SUITE_END()
