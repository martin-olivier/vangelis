#include "IO_Tester.hpp"
#include "Parsing.hpp"
#include "Utils.hpp"
#include <iostream>

void IOTester::Version() noexcept
{
    std::cout << "IO_Tester (" << VERSION << ")\n";
    std::cout << "Written by Martin OLIVIER, student at {EPITECH} Paris" << std::endl;
    exit(0);
}

void IOTester::Help(const char *bin, int returnValue) noexcept
{
    std::cout << "USAGE:\n";
    std::cout << "\t" << bin << " test.io [OPTIONS]\n\n";

    std::cout << "DESCRIPTION:\n";
    std::cout << "\ttest.io\t\tfile that contains functional tests\n\n";

    std::cout << "OPTIONS:\n";
    std::cout << "\t-h --help\tDisplay this help menu\n";
    std::cout << "\t-v --version\tDisplay the actual version\n";
    std::cout << "\t-c --changelog\tDisplay the changelog\n";
    std::cout << "\t-u --update\tUpdate this software (sudo)\n";
    std::cout << "\t--details\tDisplay the output difference in the shell\n";
    std::cout << "\t--diff\t\tDisplay the output difference in Visual Studio Code\n\n";

    std::cout << "RETURN VALUE:\n";
    std::cout << "\t0\t\tif all tests succeed\n";
    std::cout << "\t1\t\tif one or more tests failed or crashed\n";
    std::cout << "\t84\t\tif IO_Tester failed to load a test file\n";

    std::cout << std::flush;
    exit(returnValue);
}

std::vector<std::string_view> IOTester::parseArgs(int ac, char **av)
{
    std::vector<std::string_view> files{};
    bool last_arg_diff = false;

    for (int i = 1; i < ac; i++) {
        std::string_view arg(av[i]);
        if (arg == "-h" or arg == "--help")
            Help(av[0], 0);
        else if (arg == "-v" or arg == "--version")
            Version();
        else if (arg == "-u" or arg == "--update")
            Update();
        else if (arg == "-c" or arg == "--changelog")
            Changelog();
        else if (arg == "--details" and m_details == NO) {
            m_details = DETAILS;
            last_arg_diff = true;
        }
        else if (arg == "--diff" and m_details == NO) {
            m_details = DIFF;
            last_arg_diff = true;
        }
        else if (arg == "--details" or arg == "--diff")
            throw exception("bad option: please choose between --details and --diff");
        else if (arg.find('-') == 0)
            throw exception("bad option: " + std::string(arg));
        else if (last_arg_diff) {
            try {
                m_details_count = std::stoul(arg.data());
            }
            catch (...) {
                m_details_count = -1;
                files.push_back(arg);
            }
            last_arg_diff = false;
        }
        else
            files.push_back(arg);
    }
    return files;
}

IOTester::IOTester(int ac, char **av)
{
    if (ac < 2)
        Help(av[0], 84);
    auto files = parseArgs(ac, av);
    if (files.empty())
        Help(av[0], 84);

    for (size_t i = 0; i < files.size(); i++) {
        if (i != 0)
            std::cout << '\n';
        if (files.size() > 1)
            std::cout << CYN << files[i] << ":\n" << RESET << std::endl;
        m_file = Parsing::CheckFile(files[i].data());
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

    m_default_stdout = true;
    m_default_stderr = true;
    m_default_return = 0;
    m_default_timeout = 3.0;
}

void IOTester::printFinalResults() const noexcept
{
    std::cout << "\n> Tested: " << BLU << m_crashed + m_passed + m_failed + m_timeout << RESET;
    std::cout << " | Pass: " << GRN << m_passed << RESET;
    std::cout << " | Fail: " << RED << m_failed << RESET;
    std::cout << " | Crash: " << YEL << m_crashed << RESET;
    std::cout << " | Timeout: " << MAG << m_timeout << RESET << std::endl;
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