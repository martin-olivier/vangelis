#include "IO_Tester.hpp"
#include "Utils.hpp"

IOTester::IOTester(int ac, char **av) :
    m_passed(0), m_failed(0), m_crashed(0), m_position(0), m_details(false)
{
    if (ac < 2)
        ErrorHandling::Help(av[0]);
    if (strcmp(av[ac - 1], "-h") == 0 || strcmp(av[ac - 1], "--help") == 0)
        ErrorHandling::Help(av[0]);
    if (ac > 2 && (strcmp(av[ac - 1], "-d") == 0 || strcmp(av[ac - 1], "--details") == 0)) {
        m_details = true;
        ac--;
    }
    for (int i = 1; i < ac; i++) {
        if (i != 1)
            std::cout << std::endl;
        if (ac > 2)
            std::cout << CYN << av[i] << ":" << std::endl << std::endl << RESET;
        m_file = ErrorHandling::CheckFile(av[i]);
        apply();
        resetValues();
    }
}

void IOTester::resetValues()
{
    m_position = 0;
    m_passed = 0;
    m_failed = 0;
    m_crashed = 0;
}

void IOTester::apply()
{
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

    if (WEXITSTATUS(c.returnValue) == 8 || WEXITSTATUS(c.returnValue) == 11 || WEXITSTATUS(c.returnValue) == 136)
        t.m_status = Test::CRASH;
    else if (c.error) {
        t.m_status = Test::FAILED;
        return;
    }
    else
        t.m_status = Test::PASS;

    if (t.m_status == Test::CRASH) {
        std::cout << YEL << "[SF]" << RESET << " > " << t.m_name << std::endl;
        m_crashed++;
        return;
    }
    if (t.m_output == c.output) {
        std::cout << GRN << "[OK]" << RESET << " > " << t.m_name << std::endl;
        m_passed++;
    }
    else {
        std::cout << RED << "[KO]" << RESET << " > " << t.m_name << std::endl;
        m_failed++;
        if (m_details)
            std::cout << BLU << "[REAL OUTPUT] :" << RESET << std::endl << c.output << std::endl << BLU << "[EXPECTED OUTPUT] :" << RESET << std::endl << t.m_output << std::endl;
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
    IOTester(ac, av);
    return EXIT_SUCCESS;
}