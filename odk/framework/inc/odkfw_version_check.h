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

    std::vector<int> splitVersionString(const std::string& ver);

    int compareVersionStrings(const std::string& a, const std::string& b);
}
}
