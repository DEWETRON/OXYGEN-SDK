// Copyright DEWETRON GmbH 2019

#include "sdk_csv_utils.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>

void ltrim(std::string& s)
{
    auto it = std::find_if(s.begin(), s.end(),
        [](char c) {
            return !isspace(c);
        });
    s.erase(s.begin(), it);
}

void rtrim(std::string& s)
{
    auto it = std::find_if(s.rbegin(), s.rend(),
        [](char c)
        {
            return !isspace(c);
        }
    );
    s.erase(it.base(), s.end());
}

void trim(std::string& s)
{
    ltrim(s);
    rtrim(s);
}

bool isSpace(const std::string& str)
{
    for (const auto& ch : str)
    {
        if (!isspace(ch))
        {
            return false;
        }
    }
    return true;
}

std::vector<std::string> CSVNumberReader::parseLine(std::istream& input)
{
    std::vector<std::string> r;
    char ch;
    std::string cur_field;
    while (input.get(ch))
    {
        switch (ch)
        {
            case '\n':
                r.push_back(std::move(cur_field));
                return r;
            case '\r':
                r.push_back(std::move(cur_field));
                if (input.peek() == '\n')
                {
                    input.get(ch);
                }
                return r;
            case ';':
            case ',':
                r.push_back(std::move(cur_field));
                cur_field.clear();
                break;
            default:
                cur_field.push_back(ch);
                break;
        }
    }
    return r;
}

bool CSVNumberReader::parse(std::istream& input)
{
    m_columns = 0;
    m_headers.clear();
    m_values.clear();

    bool success = true;
    while (input)
    {
        const auto& fields = parseLine(input);

        // skip empty lines
        if (fields.empty() || (fields.size() == 1 && isSpace(fields.front())))
        {
            continue;
        }

        m_columns = std::max(m_columns, fields.size());

        std::vector<double> values;
        bool all_valid = true;
        values.reserve(fields.size());
        for (const auto& field : fields)
        {
            char* endptr;
            double val = strtod(field.c_str(), &endptr);
            if (endptr == field.c_str())
            {
                values.push_back(std::numeric_limits<double>::quiet_NaN());
                if (!isSpace(field))
                {
                    all_valid = false;
                }
            }
            else
            {
                values.push_back(val);
            }
        }
        if (all_valid)
        {
            m_values.push_back(std::move(values));
        }
        else
        {
            // we have no valid data => this is a header line
            if (m_values.empty() && m_headers.empty())
            {
                for (const auto& field : fields)
                {
                    std::string f = field;
                    trim(f);
                    m_headers.push_back(f);
                }
            }
            else
            {
                success = false;
                break;
            }
        }
    }
    return success;
}
