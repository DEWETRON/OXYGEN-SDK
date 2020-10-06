// Copyright DEWETRON GmbH 2019

#include "sdk_csv_utils.h"

#include <boost/algorithm/hex.hpp>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <sstream>

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
    if (!cur_field.empty())
    {
        r.push_back(std::move(cur_field));
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


std::vector<std::string> tokenize(const std::string& str, const std::string& delimiters)
{
    std::vector<std::string> tokens;

    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        tokens.emplace_back(str.substr(lastPos, pos - lastPos));

        // skip delimiters
        lastPos = str.find_first_not_of(delimiters, pos);

        // find next begin
        pos = str.find_first_of(delimiters, lastPos);
    }

    return tokens;
}

std::vector<uint8_t> AsciiHexToBits(const std::string& input)
{
    std::vector<uint8_t> result;
    result.reserve(1 + (input.length() / 2));
    boost::algorithm::unhex(input, std::back_inserter(result));
    return result;
}

bool CSVMessageReader::parse(std::istream& input)
{
    m_headers.clear();
    m_values.clear();

    std::string line;
    auto line_count = 0u;
    bool success = true;
    while (input && std::getline(input, line))
    {
        ++line_count;
        const auto& fields = tokenize(line, ",; \t\r");

        // skip empty lines
        if (fields.empty() || (fields.size() == 1 && isSpace(fields.front())))
        {
            continue;
        }

        Entry entry;
        bool valid_entry = true;
        if (fields.size() >= 2)
        {
            char* endptr;
            entry.m_time = strtod(fields[0].c_str(), &endptr);
            auto is_valid = endptr != fields[0].c_str();

            if (is_valid)
            {
                std::for_each(fields.begin() + 1, fields.end(),
                    [&entry]
                    (const std::string& val)
                    {
                        auto v = AsciiHexToBits(val);
                        std::copy(v.begin(), v.end(), std::back_inserter(entry.m_message));
                    });
                m_values.emplace_back(entry);
            }
            else if (line_count == 1)
            {
                std::copy(fields.begin(), fields.end(), std::back_inserter(m_headers));
            }
        }
    }

    return (m_values.size() + 1 <= line_count);
}

