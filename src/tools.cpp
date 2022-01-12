#include "tools.hpp"
#include "IO_Tester.hpp"
#include <fstream>
#include <dirent.h>
#include <iostream>

std::string tools::get_file_content(const std::string &path)
{
    std::string file_content{};
    std::ifstream file_stream(path);
    DIR *dir = opendir(path.c_str());
    if (dir)
        throw IOTester::exception("cannot open directory \"" + path + "\"");
    if (!file_stream.is_open())
        throw IOTester::exception("cannot open file \"" + path + "\"");
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