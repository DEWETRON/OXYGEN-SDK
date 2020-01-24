// Copyright DEWETRON GmbH 2019
#pragma once

#include <istream>
#include <string>
#include <vector>


void ltrim(std::string& s);
void rtrim(std::string& s);
void trim(std::string& s);

bool isSpace(const std::string& str);

class CSVNumberReader
{
public:

    std::vector<std::string> parseLine(std::istream& input);
    bool parse(std::istream& input);

    size_t m_columns = 0;

    std::vector<std::string> m_headers;
    std::vector<std::vector<double>> m_values;
};

