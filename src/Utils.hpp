#pragma once

#include <string>
#include <vector>

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

namespace Utils
{
    std::string get_file_content(const std::string &path);
    std::vector<std::string> string_to_vector(const std::string &str, char separator);
    void my_exit(int exit_status, const std::string &error_msg);
};