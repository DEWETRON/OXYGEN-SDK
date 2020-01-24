// Copyright DEWETRON GmbH 2019

#pragma once

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
        tuple.resize(4);

        int cnt = sscanf(ver.c_str(), "%u.%u.%u.%u", &tuple[0], &tuple[1], &tuple[2], &tuple[3]);
        if (cnt == 0 || cnt == EOF)
        {
            tuple.clear();
        }
        else
        {
            tuple.resize(cnt);
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