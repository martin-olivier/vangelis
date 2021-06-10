#include "Parsing.hpp"
#include "Utils.hpp"
#include <algorithm>

enum CheckStatus {Input, Output};

void Parsing::isInput(const std::string &line)
{
    if (line[0] != '[')
        Utils::my_exit(84, "parsing error at line :\n" + line);
    if (line.find(']') == std::string::npos)
        Utils::my_exit(84, "parsing error at line :\n" + line);
    if (line.find(']') == line.size() - 1 || line.find(']') == 1)
        Utils::my_exit(84, "parsing error at line :\n" + line);
}

void Parsing::checkIsEmpty(const std::vector<std::string> &file)
{
    if (file.empty())
        Utils::my_exit(84, "error : file is empty");
    for (auto &line : file) {
        if (!line.empty())
            return;
    }
    Utils::my_exit(84, "error : file is empty");
}

std::vector<std::string> Parsing::CheckFile(char *path)
{
    std::vector<std::string> file = Utils::string_to_vector(Utils::get_file_content(path), '\n');
    CheckStatus status = Input;

    Parsing::checkIsEmpty(file);
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
        Utils::my_exit(84, "parsing error at the last line");
    return file;
}