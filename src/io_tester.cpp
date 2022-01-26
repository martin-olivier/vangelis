#include <iostream>

#include "io_tester.hpp"
#include "parsing.hpp"
#include "format.hpp"

void io_tester::version() noexcept
{
    std::cout << "IO_Tester (" << IO_TESTER_VERSION << ")\n";
    std::cout << "Written by Martin OLIVIER, student at {EPITECH} Paris" << std::endl;
    exit(0);
}

void io_tester::help(const char *bin, int return_value) noexcept
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
    exit(return_value);
}

std::vector<std::string_view> io_tester::parse_args(int ac, char **av)
{
    std::vector<std::string_view> files{};
    bool last_arg_diff = false;

    for (int i = 1; i < ac; i++) {
        std::string_view arg(av[i]);
        if (arg == "-h" or arg == "--help")
            help(av[0], 0);
        else if (arg == "-v" or arg == "--version")
            version();
        else if (arg == "-u" or arg == "--update")
            update();
        else if (arg == "-c" or arg == "--changelog")
            changelog();
        else if (arg == "--details" and m_details == no) {
            m_details = shell;
            last_arg_diff = true;
        }
        else if (arg == "--diff" and m_details == no) {
            m_details = vsdiff;
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

io_tester::io_tester(int ac, char **av)
{
    if (ac < 2)
        help(av[0], 84);
    auto files = parse_args(ac, av);
    if (files.empty())
        help(av[0], 84);

    for (size_t i = 0; i < files.size(); i++) {
        if (i != 0)
            std::cout << '\n';
        if (files.size() > 1)
            std::cout << format::cyan << files[i] << "\n" << format::reset << std::endl;
        m_file = parsing::check_file(files[i].data());
        apply();
        reset_values();
    }
    print_results();
    if (m_details == vsdiff and !check_vscode_bin()) {
        std::cerr << format::red << "\nYou need to install Visual Studio Code to show diff" << std::endl;
        std::cerr << "Use --details otherwise" << format::reset << std::endl;
    }
    check_update();
    std::cout << std::endl;
}

void io_tester::reset_values() noexcept
{
    if (m_fail > 0 or m_crash > 0 or m_timeout > 0)
        m_return = EXIT_FAILURE;
    m_position = 0;
    m_default_stdout = true;
    m_default_stderr = true;
    m_default_return = 0;
    m_default_timeout = 3.0;
}

void io_tester::print_results() const noexcept
{
    std::cout << "\n> Tests: " << format::blue << m_crash + m_pass + m_fail + m_timeout << format::reset;
    std::cout << " | Pass: " << format::green << m_pass << format::reset;
    std::cout << " | Fail: " << format::red << m_fail << format::reset;
    std::cout << " | Crash: " << format::yellow << m_crash << format::reset;
    std::cout << " | Timeout: " << format::magenta << m_timeout << format::reset << std::endl;
}

void io_tester::apply()
{
    while (m_file[m_position].empty())
        m_position++;
    while (true) {
        comparator(get_test_data());
        while (true) {
            if (m_position >= m_file.size())
                return;
            else if (m_file[m_position].empty())
                m_position++;
            else
                break;
        }
    }
}