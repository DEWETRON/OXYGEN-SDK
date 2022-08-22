// Copyright DEWETRON GmbH 2019

#include "odkfw_version_check.h"

#include "odkuni_string_util.h"

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

namespace odk
{
namespace framework
{

    std::vector<int> splitVersionString(const std::string& ver)
    {
        std::vector<int> tuple;

        auto tokens = odk::split(ver, '.');
        for (auto t : tokens)
        {
            try
            {
                tuple.push_back(odk::from_string<int>(t));
            }
            catch (const std::logic_error&)
            {
                break;
            }
        }

        return tuple;
    }

    int compareVersionStrings(const std::string& a, const std::string& b)
    {
        auto a_tuple = splitVersionString(a);
        auto b_tuple = splitVersionString(b);

        if (a_tuple.size() < b_tuple.size())
        {
            a_tuple.resize(b_tuple.size(), 0);
        }
        if (b_tuple.size() < a_tuple.size())
        {
            b_tuple.resize(a_tuple.size(), 0);
        }

        for (size_t i = 0; i < a_tuple.size(); ++i)
        {
            if (a_tuple[i] < b_tuple[i])
            {
                return -1;
            }
            else if (a_tuple[i] > b_tuple[i])
            {
                return 1;
            }
        }
        return 0;
    }

}
}
