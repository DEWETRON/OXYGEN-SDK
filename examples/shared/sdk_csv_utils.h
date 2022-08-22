// Copyright DEWETRON GmbH 2019
#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <vector>


void ltrim(std::string& s);
void rtrim(std::string& s);
void trim(std::string& s);

bool isSpace(const std::string& str);

std::vector<uint8_t> AsciiHexToBits(const std::string& input);

class CSVNumberReader
{
public:

    std::vector<std::string> parseLine(std::istream& input);
    bool parse(std::istream& input);

    size_t m_columns = 0;

    std::vector<std::string> m_headers;
    std::vector<std::vector<double>> m_values;
};

class CSVMessageReader
{
public:

    std::vector<std::string> parseLine(std::istream& input);
    bool parse(std::istream& input);

    struct Entry
    {
        double m_time;
        std::vector<uint8_t> m_message;
    };

    std::vector<std::string> m_headers;
    std::vector<Entry> m_values;
};
