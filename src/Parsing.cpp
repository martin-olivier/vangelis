#include <algorithm>

#include "io_tester.hpp"
#include "parsing.hpp"
#include "tools.hpp"

enum check_status {input, output};

void parsing::is_empty(const std::vector<std::string> &file)
{
    if (file.empty())
        throw io_tester::exception("the file is empty");
    for (auto &line : file) {
        if (!line.empty())
            return;
    }
    throw io_tester::exception("the file is empty");
}

void parsing::is_input(const std::string &line, size_t pos)
{
    if (line[0] != '[')
        throw io_tester::exception("parsing error at line " + std::to_string(pos));
    if (line.find(']') == std::string::npos)
        throw io_tester::exception("parsing error at line " + std::to_string(pos));
    if (line.find(']') == line.size() - 1 or line.find(']') == 1)
        throw io_tester::exception("parsing error at line " + std::to_string(pos));
}

void parsing::is_param(const std::string &line, size_t pos)
{
    auto list = tools::string_to_vector(line, ' ');
    if (list[0] == "@default" and list.size() == 3) {
        list.erase(list.begin());
        list.front() = '@' + list.front();
    }
    if (list.size() != 2)
        throw io_tester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
    if (list[0] == "@stdout" or list[0] == "@stderr") {
        if (list[1] != "true" and list[1] != "false")
            throw io_tester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
    }
    else if (list[0] == "@timeout" or list[0] == "@return") {
        try {
            auto res = std::stof(list[1]);
            if (list[0] == "@timeout" and res <= 0)
                throw io_tester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
        }
        catch (...) {
            throw io_tester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
        }
        if (list[0] == "@return" and list[1].find('.') != std::string::npos)
            throw io_tester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
    }
    else
        throw io_tester::exception("bad parameter at line " + std::to_string(pos) + " : " + line);
}

std::vector<std::string> parsing::check_file(const char *path)
{
    auto file = tools::string_to_vector(tools::get_file_content(path), '\n');
    check_status status = input;
    size_t pos = 0;

    is_empty(file);
    for (auto &line : file) {
        pos += 1;
        if (line.empty())
            continue;
        else if (status == input and line[0] == '#')
            line.clear();
        else if (status == input and line[0] == '@')
            is_param(line, pos);
        else if (status == input) {
            is_input(line, pos);
            status = output;
        }
        else if (line == "[END]")
            status = input;
    }
    if (status == output)
        throw io_tester::exception("parsing error at the end of the file");
    return file;
}