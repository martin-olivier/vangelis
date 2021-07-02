#pragma once

#include <vector>
#include <string>

namespace Parsing
{
    std::vector<std::string> checkFile(const char *path);
    void isEmpty(const std::vector<std::string> &file);
    void isInput(const std::string &line, size_t pos);
    void isParam(const std::string &line, size_t pos);
}