#include "ErrorHandling.hpp"
#include "Utils.hpp"

void ErrorHandling::Help(const char *bin)
{
    std::cout << "Usage: \t\t" << bin << " [file]" << std::endl;
    std::cout << "Help menu : \t" << bin << " [file] [-h || --help]" << std::endl;
    std::cout << "Show details : \t" << bin << " [file] [-d || --details]" << std::endl;
    exit(84);
}

ErrorHandling::ErrorHandling(int ac, char **av) : m_details(false)
{
    if (ac != 2 && ac != 3)
        Help(av[0]);
    if (strcmp(av[1], "-h") == 0 || strcmp(av[1], "--help") == 0)
        Help(av[0]);
    if (ac == 3) {
        if (strcmp(av[2], "-d") == 0 || strcmp(av[2], "--details") == 0)
            m_details = true;
        else
            Help(av[0]);
    }
    m_file = Utils::string_to_vector(Utils::get_file_content(av[1]), '\n');
    CheckLoop();
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