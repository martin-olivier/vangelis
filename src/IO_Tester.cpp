#include "IO_Tester.hpp"
#include <cstring>

void IOTester::Version()
{
    std::cout << "IO_Tester (" << VERSION << ")" << std::endl;
    std::cout << "Written by Martin OLIVIER, Student at {EPITECH} Paris" << std::endl;
    exit(0);
}

IOTester::IOTester(int ac, char **av) :
    m_passed(0), m_failed(0), m_crashed(0), m_position(0), m_details(NO), m_VSCodeBin(UNCHECKED), m_return(EXIT_SUCCESS)
{
    if (ac < 2)
        ErrorHandling::Help(av[0], 84);
    if (strcmp(av[ac - 1], "-h") == 0 || strcmp(av[ac - 1], "--help") == 0)
        ErrorHandling::Help(av[0], 0);
    if (strcmp(av[ac - 1], "-v") == 0 || strcmp(av[ac - 1], "--version") == 0)
        IOTester::Version();
    if (strcmp(av[ac - 1], "-u") == 0 || strcmp(av[ac - 1], "--update") == 0)
        IOTester::Update();
    if (strcmp(av[ac - 1], "-c") == 0 || strcmp(av[ac - 1], "--changelog") == 0)
        IOTester::Changelog();
    if (ac > 2 && (strcmp(av[ac - 1], "--details") == 0)) {
        m_details = DETAILS;
        ac--;
    }
    if (ac > 2 && (strcmp(av[ac - 1], "--diff") == 0)) {
        m_details = DIFF;
        ac--;
    }
    for (int i = 1; i < ac; i++) {
        if (i != 1)
            std::cout << std::endl;
        if (ac > 2)
            std::cout << CYN << av[i] << ":\n" << RESET << std::endl;
        m_file = ErrorHandling::CheckFile(av[i]);
        apply();
        resetValues();
    }
    if (m_VSCodeBin == KO) {
        std::cerr << RED << "\nYou need to install Visual Studio Code to show diff" << std::endl;
        std::cerr << "Use --details otherwise" << RESET << std::endl;
    }
    CheckUpdate();
    std::cout << std::endl;
}

void IOTester::resetValues()
{
    if (m_failed > 0 || m_crashed > 0)
        m_return = EXIT_FAILURE;
    m_position = 0;
    m_passed = 0;
    m_failed = 0;
    m_crashed = 0;
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
    for (; m_file[m_position] != "[END]"; m_position++)
        t.m_output += m_file[m_position] + '\n';
    m_position++;
    t.m_output.pop_back();
    return t;
}

void IOTester::comparator(Test t)
{
    Utils::CMD c = Utils::get_cmd_output(t.m_cmd);

    if (c.error)
        Utils::my_exit(84, "Pipe Error, Exiting...");
    else if ((c.returnValue >= 8 && c.returnValue <= 11) || (c.returnValue >= 132 && c.returnValue <= 139))
        t.m_status = Test::CRASH;
    else if ((WEXITSTATUS(c.returnValue) >= 8 && WEXITSTATUS(c.returnValue) <= 11) || (WEXITSTATUS(c.returnValue) >= 132 && WEXITSTATUS(c.returnValue) <= 139))
        t.m_status = Test::CRASH;
    else
        t.m_status = Test::PASS;

    if (t.m_status == Test::CRASH) {
        std::cout << YEL << "[SF]" << RESET << " > " << t.m_name << std::endl;
        m_crashed++;
    }
    else if (t.m_output == c.output) {
        std::cout << GRN << "[OK]" << RESET << " > " << t.m_name << std::endl;
        m_passed++;
    }
    else {
        std::cout << RED << "[KO]" << RESET << " > " << t.m_name << std::endl;
        m_failed++;
        if (m_details == DETAILS)
            std::cout << BLU << "[GOT] :" << RESET << std::endl << c.output << std::endl << BLU << "[EXPECTED] :" << RESET << std::endl << t.m_output << std::endl;
        else if (m_details == DIFF) {
            if (m_VSCodeBin == UNCHECKED)
                checkVSCodeBin();
            if (m_VSCodeBin == OK)
                VSCodeDiff(t, c);
        }
    }
}

void IOTester::printFinalResults() const
{
    std::cout << std::endl << "> Synthesis: Tested " << BLU << m_crashed + m_passed + m_failed << RESET;
    std::cout << " | Passing: " << GRN << m_passed << RESET;
    std::cout << " | Failing: " << RED << m_failed << RESET;
    std::cout << " | Crashing: " << YEL << m_crashed << RESET << std::endl;
}

int main(int ac, char **av)
{
    IOTester app(ac, av);
    return app.exitStatus();
}