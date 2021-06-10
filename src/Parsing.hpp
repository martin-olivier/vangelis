#pragma once

#include <vector>
#include <string>

namespace Parsing
{
    std::vector<std::string> CheckFile(char *path);
    void checkIsEmpty(const std::vector<std::string> &file);
    void isInput(const std::string &line, size_t pos);
};