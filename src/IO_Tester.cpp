#include "IO_Tester.hpp"
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>

void IOTester::Version()
{
    std::cout << "IO_Tester (" << VERSION << ")" << std::endl;
    std::cout << "Written by Martin OLIVIER, student at {EPITECH} Paris" << std::endl;
    exit(0);
}

IOTester::IOTester(int ac, char **av) :
    m_passed(0), m_failed(0), m_crashed(0), m_timeout(0), m_position(0), m_details(NO), m_return(EXIT_SUCCESS), m_timeout_value(3.0)
{
    int i = 1;
    if (ac < 2)
        ErrorHandling::Help(av[0], 84);
    if (std::string_view(av[1]) == "-t" or std::string_view(av[1]) == "--timeout") {
        if (ac == 2)
            ErrorHandling::Help(av[0], 84);
        try {m_timeout_value = std::stof(av[2]);}
        catch (...) {Utils::my_exit(84, "Invalid timeout argument : IO_Tester " + std::string(av[1]) + " <time in seconds> test.io");}
        if (m_timeout_value < 0)
            Utils::my_exit(84, "Invalid timeout argument : IO_Tester " + std::string(av[1]) + " <time in seconds> test.io");
        i+=2;
        if (i >= ac)
            return;
    }
    std::string_view last_arg(av[ac - 1]);
    if (last_arg == "-h" or last_arg == "--help")
        ErrorHandling::Help(av[0], 0);
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
        m_file = ErrorHandling::CheckFile(av[i]);
        apply();
        resetValues();
    }
    if (m_details == DIFF and !checkVSCodeBin()) {
        std::cerr << RED << "\nYou need to install Visual Studio Code to show diff" << std::endl;
        std::cerr << "Use --details otherwise" << RESET << std::endl;
    }
    CheckUpdate();
    std::cout << std::endl;
}

void IOTester::resetValues()
{
    if (m_failed > 0 or m_crashed > 0 or m_timeout > 0)
        m_return = EXIT_FAILURE;
    m_position = 0;
    m_passed = 0;
    m_failed = 0;
    m_crashed = 0;
    m_timeout = 0;
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
    for (; m_file[m_position].find("[END]") != 0; m_position++)
        t.m_output += m_file[m_position] + '\n';
    std::string ret_val = m_file[m_position].substr(5);
    try {t.m_return_value = std::stoi(ret_val);}
    catch (...) {Utils::my_exit(84, "error : bad expected return value : " + ret_val);}
    m_position++;
    if (!t.m_output.empty())
        t.m_output.pop_back();
    return t;
}

void display(Test t, const std::string &output, int returnValue, IOTester::Details details)
{
    if ((returnValue >= 8 and returnValue <= 11) or (returnValue >= 132 and returnValue <= 139))
        t.m_status = Test::CRASH;
    else if ((WEXITSTATUS(returnValue) >= 8 and WEXITSTATUS(returnValue) <= 11) or (WEXITSTATUS(returnValue) >= 132 and WEXITSTATUS(returnValue) <= 139))
        t.m_status = Test::CRASH;
    else
        t.m_status = Test::PASS;

    if (t.m_status == Test::CRASH)
        std::cout << YEL << "[SF]" << RESET << " > " << t.m_name << std::endl;
    else if (t.m_output == output and t.m_return_value == WEXITSTATUS(returnValue))
        std::cout << GRN << "[OK]" << RESET << " > " << t.m_name << std::endl;
    else {
        std::cout << RED << "[KO]" << RESET << " > " << t.m_name << std::endl;
        t.m_status = Test::FAILED;
        if (details == IOTester::DETAILS) {
            if (t.m_output == output)
                std::cout << BLU << "[GOT] :\n" << RESET << "Return Value -> " << WEXITSTATUS(returnValue) << BLU << "\n[EXPECTED] :\n" << RESET << "Return Value -> " << t.m_return_value << std::endl;
            else
                std::cout << BLU << "[GOT] :\n" << RESET << output << BLU << "\n[EXPECTED] :\n" << RESET << t.m_output << std::endl;
        }
        else if (details == IOTester::DIFF) {
            std::string out = output;
            if (t.m_output == output) {
                t.m_output = "Return Value -> " + std::to_string(t.m_return_value);
                out = "Return Value -> " + std::to_string(WEXITSTATUS(returnValue));
            }
            if (IOTester::checkVSCodeBin())
                IOTester::VSCodeDiff(t, out);
        }
    }
    _exit(t.m_status);
}

void compute(Test test, pid_t pid, int &status, IOTester::Details details)
{
    char buffer[2048];

    if (pid == -1) {
        status = Test::ERROR;
        return;
    } else if (pid == 0) {
        std::string output;
        int return_value;
        FILE *pipe = popen(std::string("exec 2>&1 ; " + test.m_cmd).c_str(), "r");
        if (!pipe)
            _exit(Test::ERROR);
        while (!feof(pipe)) {
            if (fgets(buffer, 2048, pipe) != NULL)
                output += buffer;
        }
        return_value = pclose(pipe);
        display(test, output, return_value, details);
    } else {
        int ret = 0;
        waitpid(pid, &ret, WUNTRACED | WCONTINUED);
        status = WEXITSTATUS(ret);
        if (WIFSIGNALED(ret))
            status = Test::TIMEOUT;
    }
}

void IOTester::comparator(Test t)
{
    pid_t pid = fork();
    int ret = Test::NIL;
    std::thread th(compute, t, pid, std::ref(ret), m_details);

    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::microseconds(static_cast<int>(m_timeout_value * 1000000));
    while (true) {
        if (std::chrono::system_clock::now() >= deadline)
            break;
        if (ret != Test::NIL)
            break;
    }
    kill(pid, SIGKILL);
    th.join();
    if (ret == Test::PASS)
        m_passed++;
    else if (ret == Test::FAILED)
        m_failed++;
    else if (ret == Test::CRASH)
        m_crashed++;
    else if (ret == Test::TIMEOUT) {
        std::cout << MAG << "[TO]" << RESET << " > " << t.m_name << std::endl;
        m_timeout++;
    }
    else if (ret == Test::ERROR)
        Utils::my_exit(84, "pipe error, exiting...");
}

void IOTester::printFinalResults() const
{
    std::cout << std::endl << "> Synthesis: Tested: " << BLU << m_crashed + m_passed + m_failed + m_timeout << RESET;
    std::cout << " | Pass: " << GRN << m_passed << RESET;
    std::cout << " | Fail: " << RED << m_failed << RESET;
    std::cout << " | Crash: " << YEL << m_crashed << RESET;
    std::cout << " | Timeout: " << MAG << m_timeout << RESET << std::endl;
}

int main(int ac, char **av)
{
    IOTester app(ac, av);
    return app.exitStatus();
}