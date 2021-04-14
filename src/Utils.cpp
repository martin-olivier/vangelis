#include "Utils.hpp"
#include <fstream>
#include <dirent.h>
#include <iostream>

void Utils::my_exit(int exit_status, const std::string &error_msg)
{
    std::cerr << error_msg << std::endl;
    exit(exit_status);
}

std::string Utils::get_file_content(const std::string &path)
{
    std::string file_content;
    std::ifstream file_stream(path);
    DIR *dir = opendir(path.c_str());
    if (dir)
        my_exit(84, "Cannot open directory [" + path + "], exiting...");

    if (!file_stream.is_open())
        my_exit(84, "Cannot open file [" + path + "], exiting...");
    std::getline(file_stream, file_content, '\0');
    return file_content;
}

std::vector<std::string> Utils::string_to_vector(const std::string &str, char separator)
{
    std::vector<std::string> array;
    std::string temp;
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