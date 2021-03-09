#pragma once

#include <vector>
#include <string>

namespace ErrorHandling
{
    std::vector<std::string> CheckFile(char *path);
    void checkIsEmpty(const std::vector<std::string> &file);
    void isInput(const std::string &line);
    void Help(const char *bin, int returnValue);
};