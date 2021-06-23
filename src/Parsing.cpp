#include "IO_Tester.hpp"
#include "Parsing.hpp"
#include "Utils.hpp"
#include <algorithm>

enum CheckStatus {Input, Output};

void Parsing::checkIsEmpty(const std::vector<std::string> &file)
{
    if (file.empty())
        throw IOTester::exception("the file is empty");
    for (auto &line : file) {
        if (!line.empty())
            return;
    }
    throw IOTester::exception("the file is empty");
}

void Parsing::isInput(const std::string &line, size_t pos)
{
    if (line[0] != '[')
        throw IOTester::exception("parsing error at line " + std::to_string(pos));
    if (line.find(']') == std::string::npos)
        throw IOTester::exception("parsing error at line : " + std::to_string(pos));
    if (line.find(']') == line.size() - 1 or line.find(']') == 1)
        throw IOTester::exception("parsing error at line : " + std::to_string(pos));
}

void Parsing::isParam(const std::string &line, size_t pos)
{
    auto list = Utils::string_to_vector(line, ' ');
    if (list[0] == "@default" and list.size() == 3) {
        list.erase(list.begin());
        list.front() = '@' + list.front();
    }
    if (list.size() != 2)
        throw IOTester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
    if (list[0] == "@stdout" or list[0] == "@stderr") {
        if (list[1] != "true" and list[1] != "false")
            throw IOTester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
    }
    else if (list[0] == "@timeout" or list[0] == "@return") {
        try {
            auto res = std::stof(list[1]);
            if (list[0] == "@timeout" and res <= 0)
                throw IOTester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
        }
        catch (...) {
            throw IOTester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
        }
        if (list[0] == "@return" and list[1].find('.') != std::string::npos)
            throw IOTester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
    }
    else
        throw IOTester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
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
        else if (status == Input and line[0] == '@')
            isParam(line, pos);
        else if (status == Input) {
            isInput(line, pos);
            status = Output;
        }
        else if (line == "[END]")
            status = Input;
    }
    if (status == Output)
        throw IOTester::exception("parsing error at the end of the file");
    return file;
}