#include "ErrorHandling.hpp"
#include "Utils.hpp"

ErrorHandling::ErrorHandling(int ac, char **av) : m_details(false)
{
    if (ac != 2 && ac != 3)
        exit(84);
    if (ac == 3) {
        if (strcmp(av[2], "-details") == 0)
            m_details = true;
    }
    m_file = Utils::string_to_vector(Utils::get_file_content(av[1]), '\n');
    this->CheckLoop();
}

enum CheckStatus {Input, Output};

void ErrorHandling::isInput(const std::string &line)
{
    if (line[0] != '[')
        Utils::my_exit(84, "Parsing Error at line :\n" + line);
    if (line.find(']') == std::string::npos)
        Utils::my_exit(84, "Parsing Error at line :\n" + line);
    if (line.find(']') == line.size() - 1 || line.find(']') == 1)
        Utils::my_exit(84, "Parsing Error at line :\n" + line);
}

void ErrorHandling::CheckLoop()
{
    CheckStatus status = Input;

    if (m_file.empty())
        Utils::my_exit(84, "Error : File is Empty");
    for (const auto& line : m_file) {
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
}