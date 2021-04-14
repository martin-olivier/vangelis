#include "ErrorHandling.hpp"
#include "Utils.hpp"
#include <iostream>
#include <algorithm>

enum CheckStatus {Input, Output};

void ErrorHandling::Help(const char *bin, int returnValue)
{
    std::cout << "USAGE:" << std::endl;
    std::cout << "\t" << bin << " test.io [OPTIONS]\n" << std::endl;

    std::cout << "DESCRIPTION:" << std::endl;
    std::cout << "\ttest.io\t\tfile that contains functional tests\n" << std::endl;

    std::cout << "OPTIONS:" << std::endl;
    std::cout << "\t-t --timeout\tChange the tests timeout" << std::endl;
    std::cout << "\t\t\tmust be the first argument followed by the value in seconds as second argument" << std::endl;
    std::cout << "\t-h --help\tDisplay help menu" << std::endl;
    std::cout << "\t-v --version\tDisplay actual version" << std::endl;
    std::cout << "\t-c --changelog\tDisplay the changelog" << std::endl;
    std::cout << "\t-u --update\tUpdate this software (sudo)" << std::endl;
    std::cout << "\t--details\tDisplay details of all tests" << std::endl;
    std::cout << "\t--diff\t\tDisplay difference in VSCode\n" << std::endl;

    std::cout << "RETURN VALUE:" << std::endl;
    std::cout << "\t0\t\tif all tests succeed" << std::endl;
    std::cout << "\t1\t\tif one or more tests failed or crashed" << std::endl;
    std::cout << "\t84\t\tif IO_Tester failed to load the test file" << std::endl;

    exit(returnValue);
}

void ErrorHandling::isInput(const std::string &line)
{
    if (line[0] != '[')
        Utils::my_exit(84, "Parsing Error at line :\n" + line);
    if (line.find(']') == std::string::npos)
        Utils::my_exit(84, "Parsing Error at line :\n" + line);
    if (line.find(']') == line.size() - 1 || line.find(']') == 1)
        Utils::my_exit(84, "Parsing Error at line :\n" + line);
}

void ErrorHandling::checkIsEmpty(const std::vector<std::string> &file)
{
    if (file.empty())
        Utils::my_exit(84, "Error : File is Empty");
    for (auto &line : file) {
        if (!line.empty())
            return;
    }
    Utils::my_exit(84, "Error : File is Empty");
}

std::vector<std::string> ErrorHandling::CheckFile(char *path)
{
    std::vector<std::string> file = Utils::string_to_vector(Utils::get_file_content(path), '\n');
    CheckStatus status = Input;

    ErrorHandling::checkIsEmpty(file);
    for (auto& line : file) {
        if (line.empty())
            continue;
        else if (status == Input && line[0] == '#')
            line.clear();
        else if (status == Input) {
            isInput(line);
            status = Output;
        }
        else if (status == Output) {
            if (line.find("[END]") == 0) {
                status = Input;
                line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
                std::string ret_val = line.substr(5);
                if (!ret_val.empty() and ret_val[0] == '>')
                    ret_val.erase(ret_val.begin());
                else if (ret_val.size() >= 2 and ret_val[0] == '-' and ret_val[1] == '>') {
                    ret_val.erase(ret_val.begin());
                    ret_val.erase(ret_val.begin());
                }
                if (ret_val.empty())
                    ret_val.push_back('0');
                line = "[END]" + ret_val;
            }
        }
    }
    if (status == Output)
        Utils::my_exit(84, "Parsing Error at the last line");
    return file;
}