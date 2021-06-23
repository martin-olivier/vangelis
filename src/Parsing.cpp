#include "IO_Tester.hpp"
#include "Parsing.hpp"
#include "Utils.hpp"
#include <algorithm>

enum CheckStatus {Input, Output};

void Parsing::isInput(const std::string &line, size_t pos)
{
    if (line[0] != '[')
        throw IOTester::exception("parsing error at line " + std::to_string(pos));
    if (line.find(']') == std::string::npos)
        throw IOTester::exception("parsing error at line : " + std::to_string(pos));
    if (line.find(']') == line.size() - 1 || line.find(']') == 1)
        throw IOTester::exception("parsing error at line : " + std::to_string(pos));
}

void Parsing::checkIsEmpty(const std::vector<std::string> &file)
{
    if (file.empty())
        throw IOTester::exception("empty test file");
    for (auto &line : file) {
        if (!line.empty())
            return;
    }
    throw IOTester::exception("empty test file");
}

std::vector<std::string> Parsing::CheckFile(const char *path)
{
    std::vector<std::string> file = Utils::string_to_vector(Utils::get_file_content(path), '\n');
    CheckStatus status = Input;
    size_t pos = 0;

    Parsing::checkIsEmpty(file);
    for (auto &line : file) {
        pos += 1;
        if (line.empty())
            continue;
        else if (status == Input and line[0] == '#')
            line.clear();
        else if (status == Input) {
            isInput(line, pos);
            status = Output;
        }
        else if (status == Output and line.find("[END]") == 0) {
            status = Input;
            line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
            std::string ret_val = line.substr(5);
            if (ret_val.empty())
                ret_val.push_back('0');
            try {std::stoi(ret_val);}
            catch (...) {throw IOTester::exception("bad expected return value \"" + ret_val + "\" at line " + std::to_string(pos));}
            line = "[END]" + ret_val;
        }
    }
    if (status == Output)
        throw IOTester::exception("parsing error at the end of the file");
    return file;
}