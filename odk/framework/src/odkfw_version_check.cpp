// Copyright DEWETRON GmbH 2019

#include <algorithm>
#include <cctype>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

namespace odk
{
namespace framework
{

    std::vector<int> splitVersionString(const std::string& ver)
    {
        std::vector<int> tuple;

        boost::char_separator<char> sep(".");
        boost::tokenizer< boost::char_separator<char>> tok(ver, sep);

        for (auto t : tok)
        {
            try
            {
                tuple.push_back(boost::lexical_cast<int>(t));
            }
            catch (const boost::bad_lexical_cast&)
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
