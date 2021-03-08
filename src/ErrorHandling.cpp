#include "ErrorHandling.hpp"
#include "Utils.hpp"

enum CheckStatus {Input, Output};

void ErrorHandling::Help(const char *bin, int returnValue)
{
    std::cout << "USAGE:" << std::endl;
    std::cout << "\t" << bin << " test.io [OPTIONS]\n" << std::endl;
    std::cout << "DESCRIPTION:" << std::endl;
    std::cout << "\ttest.io\t\tfile when contains all tests functional\n" << std::endl;

    std::cout << "OPTIONS:" << std::endl;
    std::cout << "\t-h --help\tDisplay this help" << std::endl;
    std::cout << "\t-v --version\tDisplay this version" << std::endl;
    std::cout << "\t-c --changelog\tDisplay this changelog" << std::endl;
    std::cout << "\t-u --update\tUpdate this programm (sudo)" << std::endl;
    std::cout << "\t--details\tDisplay details of all tests" << std::endl;
    std::cout << "\t--diff\t\tDisplay difference in VSCode" << std::endl;

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

std::vector<std::string> ErrorHandling::CheckFile(char *path)
{
    std::vector<std::string> file = Utils::string_to_vector(Utils::get_file_content(path), '\n');
    CheckStatus status = Input;

    if (file.empty())
        Utils::my_exit(84, "Error : File is Empty");
    for (const auto& line : file) {
        if (line.empty())
            continue;
        else if (status == Input) {
            isInput(line);
            status = Output;
        }
        else if (status == Output) {
            if (line == "[END]")
                status = Input;
        }
    }
    if (status == Output)
        Utils::my_exit(84, "Parsing Error at the last line");
    return file;
}