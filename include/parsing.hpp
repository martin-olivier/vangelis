#pragma once

#include <vector>
#include <string>

namespace parsing
{
    std::vector<std::string> check_file(const char *path);
    void is_empty(const std::vector<std::string> &file);
    void is_input(const std::string &line, size_t pos);
    void is_param(const std::string &line, size_t pos);
}