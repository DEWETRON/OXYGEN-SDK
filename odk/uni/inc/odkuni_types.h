// Copyright (c) DEWETRON GmbH 2022
#pragma once

#include <iostream>
#include <optional> //C++17
#include <string_view> //C++17

namespace odk
{
    template <typename IntType>
    class rational
    {
    public:
        rational()
        {
            assign(0, 1);
        }

        rational(const IntType& num, const IntType& den = 1)
        {
            assign(num, den);
        }

        rational(const rational<IntType>& r)
        {
            assign(r.numerator(), r.denominator());
        }

        const IntType& numerator() const
        {
            return m_numerator;
        }

        const IntType& denominator() const
        {
            return m_denominator;
        }

        void assign(const IntType& num, const IntType& den)
        {
            m_numerator = num;
            m_denominator = den;
        }

        bool operator==(const rational<IntType>& other) const
        {
            return m_numerator == other.numerator()
                && m_denominator == other.denominator();
        }

    private:
        IntType m_numerator;
        IntType m_denominator;
    };

    template <typename IntType>
    std::ostream& operator<<(std::ostream& os, const rational<IntType>& r)
    {
        os << r.numerator() << '/' << r.denominator();
        return os;
    }

}
