#pragma once

#include <string>
#include <vector>

namespace tools
{
    std::string get_file_content(const std::string &path);
    std::vector<std::string> string_to_vector(const std::string &str, char separator);
}