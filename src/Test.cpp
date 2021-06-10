#include "IO_Tester.hpp"
#include "Utils.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>

void IOTester::display(Test test, const std::string &output, int returnValue, Details details)
{
    if ((returnValue >= 8 and returnValue <= 11) or (returnValue >= 132 and returnValue <= 139))
        test.m_status = Test::CRASH;
    else if ((WEXITSTATUS(returnValue) >= 8 and WEXITSTATUS(returnValue) <= 11) or (WEXITSTATUS(returnValue) >= 132 and WEXITSTATUS(returnValue) <= 139))
        test.m_status = Test::CRASH;
    else
        test.m_status = Test::PASS;

    if (test.m_status == Test::CRASH)
        std::cout << YEL << "[SF]" << RESET << " > " << test.m_name << std::endl;
    else if (test.m_output == output and test.m_return_value == WEXITSTATUS(returnValue))
        std::cout << GRN << "[OK]" << RESET << " > " << test.m_name << std::endl;
    else {
        std::cout << RED << "[KO]" << RESET << " > " << test.m_name << std::endl;
        test.m_status = Test::FAILED;
        if (details == IOTester::DETAILS) {
            if (test.m_output == output)
                std::cout << BLU << "[GOT] :\n" << RESET << "Return Value -> " << WEXITSTATUS(returnValue) << BLU << "\n[EXPECTED] :\n" << RESET << "Return Value -> " << test.m_return_value << std::endl;
            else
                std::cout << BLU << "[GOT] :\n" << RESET << output << BLU << "\n[EXPECTED] :\n" << RESET << test.m_output << std::endl;
        }
        else if (details == IOTester::DIFF) {
            std::string out = output;
            if (test.m_output == output) {
                test.m_output = "Return Value -> " + std::to_string(test.m_return_value);
                out = "Return Value -> " + std::to_string(WEXITSTATUS(returnValue));
            }
            if (IOTester::CheckVSCodeBin())
                IOTester::VSCodeDiff(test, out);
        }
    }
    _exit(test.m_status);
}

void IOTester::compute(const Test &test, pid_t pid, int &status, Details details)
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

void IOTester::comparator(const Test &test)
{
    pid_t pid = fork();
    int ret = Test::NIL;
    std::thread proc(compute, std::cref(test), pid, std::ref(ret), m_details);

    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now() + std::chrono::microseconds(static_cast<int>(m_timeout_value * 1000000));
    while (true) {
        if (std::chrono::system_clock::now() >= deadline)
            break;
        if (ret != Test::NIL)
            break;
    }
    kill(pid, SIGKILL);
    proc.join();
    if (ret == Test::PASS)
        m_passed++;
    else if (ret == Test::FAILED)
        m_failed++;
    else if (ret == Test::CRASH)
        m_crashed++;
    else if (ret == Test::TIMEOUT) {
        std::cout << MAG << "[TO]" << RESET << " > " << test.m_name << std::endl;
        m_timeout++;
    }
    else if (ret == Test::ERROR)
        throw exception("pipe failed");
}