#include "IO_Tester.hpp"
#include "Parsing.hpp"
#include "Utils.hpp"
#include <iostream>
#include <string_view>

void IOTester::Version() noexcept
{
    std::cout << "IO_Tester (" << VERSION << ")" << std::endl;
    std::cout << "Written by Martin OLIVIER, student at {EPITECH} Paris" << std::endl;
    exit(0);
}

void IOTester::Help(const char *bin, int returnValue) noexcept
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
    std::cout << "\t84\t\tif IO_Tester failed to load a test file" << std::endl;

    exit(returnValue);
}

IOTester::IOTester(int ac, char **av)
{
    int i = 1;
    if (ac < 2)
        IOTester::Help(av[0], 84);
    if (std::string_view(av[1]) == "-t" or std::string_view(av[1]) == "--timeout") {
        if (ac == 2)
            IOTester::Help(av[0], 84);
        try {m_timeout_value = std::stof(av[2]);}
        catch (...) {throw exception("invalid timeout argument : IO_Tester " + std::string(av[1]) + " <time in seconds> test.io");}
        if (m_timeout_value < 0)
            throw exception("invalid timeout argument : IO_Tester " + std::string(av[1]) + " <time in seconds> test.io");
        i += 2;
        if (i >= ac)
            return;
    }
    std::string_view last_arg(av[ac - 1]);
    if (last_arg == "-h" or last_arg == "--help")
        IOTester::Help(av[0], 0);
    if (last_arg == "-v" or last_arg == "--version")
        IOTester::Version();
    if (last_arg == "-u" or last_arg == "--update")
        IOTester::Update();
    if (last_arg == "-c" or last_arg == "--changelog")
        IOTester::Changelog();
    if (ac > 2 and last_arg == "--details") {
        m_details = DETAILS;
        ac--;
    }
    if (ac > 2 and last_arg == "--diff") {
        m_details = DIFF;
        ac--;
    }
    for (int loop = 0; i < ac; i++, loop++) {
        if (loop != 0)
            std::cout << std::endl;
        if (ac > 2 and !(std::string_view(av[1]) == "-t" or std::string_view(av[1]) == "--timeout"))
            std::cout << CYN << av[i] << ":\n" << RESET << std::endl;
        if (ac > 4 and (std::string_view(av[1]) == "-t" or std::string_view(av[1]) == "--timeout"))
            std::cout << CYN << av[i] << ":\n" << RESET << std::endl;
        m_file = Parsing::CheckFile(av[i]);
        apply();
        resetValues();
    }
    if (m_details == DIFF and !CheckVSCodeBin()) {
        std::cerr << RED << "\nYou need to install Visual Studio Code to show diff" << std::endl;
        std::cerr << "Use --details otherwise" << RESET << std::endl;
    }
    CheckUpdate();
    std::cout << std::endl;
}

void IOTester::resetValues() noexcept
{
    if (m_failed > 0 or m_crashed > 0 or m_timeout > 0)
        m_return = EXIT_FAILURE;
    m_position = 0;
    m_passed = 0;
    m_failed = 0;
    m_crashed = 0;
    m_timeout = 0;
}

void IOTester::printFinalResults() const noexcept
{
    std::cout << "\n> Synthesis: Tested: " << BLU << m_crashed + m_passed + m_failed + m_timeout << RESET;
    std::cout << " | Pass: " << GRN << m_passed << RESET;
    std::cout << " | Fail: " << RED << m_failed << RESET;
    std::cout << " | Crash: " << YEL << m_crashed << RESET;
    std::cout << " | Timeout: " << MAG << m_timeout << RESET << std::endl;
}

Test IOTester::getTestData()
{
    Test t;
    std::string line = m_file[m_position];
    size_t pos = 1;
    for (; pos < line.size(); pos++) {
        if (line[pos] == ']')
            break;
        else
            t.m_name += line[pos];
    }
    pos++;
    for (; pos < line.size(); pos++)
        t.m_cmd += line[pos];
    m_position++;
    for (; m_file[m_position].find("[END]") != 0; m_position++)
        t.m_output += m_file[m_position] + '\n';
    std::string ret_val = m_file[m_position].substr(5);
    t.m_return_value = std::stoi(ret_val);
    m_position++;
    if (!t.m_output.empty())
        t.m_output.pop_back();
    return t;
}

void IOTester::apply()
{
    while (m_file[m_position].empty())
        m_position++;
    while (true) {
        comparator(getTestData());
        while (true) {
            if (m_position >= m_file.size()) {
                printFinalResults();
                return;
            }
            else if (m_file[m_position].empty())
                m_position++;
            else
                break;
        }
    }
}

int main(int ac, char **av)
{
    try {
        IOTester app(ac, av);
        return app.exitStatus();
    }
    catch (const std::exception &e) {
        std::cerr << RED << "error: " << RESET << e.what() << std::endl;
        return 84;
    }
    catch (...) {
        std::cerr << RED << "error: " << RESET << "unknown exception caught" << std::endl;
        return 84;
    }
}