#include <filesystem>
#include <fstream>
#include <iostream>

#include "io_tester.hpp"
#include "tools.hpp"

std::string tools::get_file_content(const std::string &path)
{
    std::string file_content{};
    std::ifstream file_stream(path);
    if (std::filesystem::is_directory(path))
        throw io_tester::exception("cannot open directory \"" + path + "\"");
    if (!file_stream.is_open())
        throw io_tester::exception("cannot open file \"" + path + "\"");
    std::getline(file_stream, file_content, '\0');
    return file_content;
}

std::vector<std::string> tools::string_to_vector(const std::string &str, char separator)
{
    std::vector<std::string> array{};
    std::string temp{};
    size_t len = str.size();

    for (size_t i = 0; i < len; i++) {
        if (str[i] == separator) {
            array.push_back(temp);
            temp.clear();
        }
        else
            temp.push_back(str[i]);
    }
    array.push_back(temp);
    return array;
}