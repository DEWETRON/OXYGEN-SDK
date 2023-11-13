// Copyright DEWETRON GmbH 2022

#include "sdk_csv_utils.h"

#include <tuple>
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(sdk_csv_utils_test_suite)

BOOST_AUTO_TEST_CASE(AsciiHexToBitsTest)
{

	std::vector<std::tuple<std::string, std::vector<uint8_t>>> cases =
	{
		{"", {}},
		{"1", {}},
		{"01", {1}},
		{"013", {1}},
		{"000102030405060708090a0b0c0d0e0f0A0B0C0D0E0F", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 10, 11, 12, 13, 14, 15}},
		{"h1", {}},
		{"1H", {}},
	};

	for (auto c : cases)
	{
		std::vector<uint8_t> expected = std::get<1>(c);
		auto result = AsciiHexToBits(std::get<0>(c));
		BOOST_CHECK_EQUAL_COLLECTIONS(result.begin(), result.end(), expected.begin(), expected.end());
	}
}

BOOST_AUTO_TEST_SUITE_END()
