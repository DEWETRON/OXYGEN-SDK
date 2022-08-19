#pragma once

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace odk
{

    inline bool strequal(const char* a, const char* b)
    {
        if (a == b)
        {
            return true;
        }
        else if (a && b)
        {
            return std::strcmp(a, b) == 0;
        }
        else
        {
            return false;
        }
    }

    inline void ltrim(std::string& s)
    {
        auto it = std::find_if(s.begin(), s.end(),
            [](char c) {
                return !isspace(c);
            });
        s.erase(s.begin(), it);
    }

    inline void rtrim(std::string& s)
    {
        auto it = std::find_if(s.rbegin(), s.rend(),
            [](char c)
            {
                return !isspace(c);
            }
        );
        s.erase(it.base(), s.end());
    }

    inline void trim(std::string& s)
    {
        ltrim(s);
        rtrim(s);
    }


    /// throws a subclass of std::logic_error if parsing fails
    template <typename T>
    inline T from_string(const std::string& val);

    template <>
    inline std::uint64_t from_string<std::uint64_t>(const std::string& val)
    {
        return std::stoull(val, nullptr, 0);
    }

    template <>
    inline std::int64_t from_string<std::int64_t>(const std::string& val)
    {
        return std::stoll(val, nullptr, 0);
    }

    template <>
    inline std::uint32_t from_string<std::uint32_t>(const std::string& val)
    {
        return std::stoul(val, nullptr, 0);
    }

    template <>
    inline std::int32_t from_string<std::int32_t>(const std::string& val)
    {
        return std::stol(val, nullptr, 0);
    }

    template <>
    inline double from_string<double>(const std::string& val)
    {
        return std::stod(val, nullptr);
    }


    template <typename T>
    inline std::string to_string(T value)
    {
        return std::to_string(value);
    }

    template <>
    inline std::string to_string(float value)
    {
        std::ostringstream os;
        os << std::setprecision(std::numeric_limits<float>::max_digits10) << value;
        return os.str();
    }

    template <>
    inline std::string to_string(double value)
    {
        std::ostringstream os;
        os << std::setprecision(std::numeric_limits<double>::max_digits10) << value;
        return os.str();
    }

    template <>
    inline std::string to_string(long double value)
    {
        std::ostringstream os;
        os << std::setprecision(std::numeric_limits<long double>::max_digits10) << value;
        return os.str();
    }


    template<typename T>
    inline ::std::vector<T> split(const T& str, const typename T::value_type& sep)
    {
        ::std::vector<T> r;
        if (!str.empty())
        {
            auto start = str.begin();
            do
            {
                auto next = ::std::find(start, str.end(), sep);
                r.insert(r.end(), T{ start, next });
                start = next;
            } while (start != str.end()
                && ++start != str.begin()); // make sure we do not increment str.end()!
        }
        return r;
    }

}
