#include "IO_Tester.hpp"
#include "Utils.hpp"
#include <iostream>
#include <functional>
#include <map>
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
        std::cout << YEL << "[SF]" << RESET << ' ' << test.m_name << std::endl;
    else if (test.m_output == output and test.m_return == WEXITSTATUS(returnValue))
        std::cout << GRN << "[OK]" << RESET << ' ' << test.m_name << std::endl;
    else {
        std::cout << RED << "[KO]" << RESET << ' ' << test.m_name << std::endl;
        test.m_status = Test::FAILED;
        if (details == IOTester::DETAILS) {
            if (test.m_output == output)
                std::cout << BLU << "[GOT] :\n" << RESET << "Return Value -> " << WEXITSTATUS(returnValue) << BLU << "\n[EXPECTED] :\n" << RESET << "Return Value -> " << test.m_return << std::endl;
            else
                std::cout << BLU << "[GOT] :\n" << RESET << output << BLU << "\n[EXPECTED] :\n" << RESET << test.m_output << std::endl;
        }
        else if (details == IOTester::DIFF) {
            std::string out = output;
            if (test.m_output == output) {
                test.m_output = "Return Value -> " + std::to_string(test.m_return);
                out = "Return Value -> " + std::to_string(WEXITSTATUS(returnValue));
            }
            if (IOTester::checkVSCodeBin())
                IOTester::VSCodeDiff(test, out);
        }
    }
    _exit(test.m_status);
}

std::string IOTester::getCMD(const Test &test)
{
    std::string cmd{};
    if (test.m_stdout and test.m_stderr)
        cmd = "exec 2>&1 ; ";
    else if (!test.m_stdout and !test.m_stderr)
        cmd = "exec 1>/dev/null ; exec 2>/dev/null ; ";
    else if (!test.m_stdout)
        cmd = "exec 3>&1 1>/dev/null 2>&3 ; ";
    else if (!test.m_stderr)
        cmd = "exec 2>/dev/null ; ";
    return cmd + test.m_cmd;
}

void IOTester::compute(const Test &test, pid_t pid, int &status, Details details)
{
    char buffer[2048];

    if (pid == -1) {
        status = Test::ERROR;
        return;
    } else if (pid == 0) {
        std::string output{};
        int return_value;
        FILE *pipe = popen(getCMD(test).c_str(), "r");
        if (!pipe)
            _exit(Test::ERROR);
        while (!feof(pipe)) {
            if (fgets(buffer, 2048, pipe))
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
    int ret = Test::NIL;
    Details det = m_details;

    if (m_details_count == 0)
        det = NO;
    pid_t pid = fork();
    std::thread proc(compute, std::cref(test), pid, std::ref(ret), det);

    std::chrono::system_clock::time_point deadline = std::chrono::system_clock::now();
    deadline += std::chrono::microseconds(static_cast<int>(test.m_timeout * 1000000));
    while (true) {
        if (std::chrono::system_clock::now() >= deadline)
            break;
        if (ret != Test::NIL)
            break;
    }
    if (ret == Test::NIL)
        kill(pid, SIGKILL);
    proc.join();
    if (ret == Test::PASS)
        m_passed++;
    else if (ret == Test::FAILED) {
        m_failed++;
        m_details_count -= 1;
    }
    else if (ret == Test::CRASH)
        m_crashed++;
    else if (ret == Test::TIMEOUT) {
        std::cout << MAG << "[TO]" << RESET << ' ' << test.m_name << std::endl;
        m_timeout++;
    }
    else if (ret == Test::ERROR)
        throw exception("pipe failed");
}

Test IOTester::getTestData()
{
    Test test(m_default_stdout, m_default_stderr, m_default_return, m_default_timeout);

    std::map<std::string_view, std::function<void(const std::string &)>> defaultParamMap {
        {"stdout", [&](const std::string &val) {test.m_stdout = m_default_stdout = (val == "true");}},
        {"stderr", [&](const std::string &val) {test.m_stderr = m_default_stderr = (val == "true");}},
        {"return", [&](const std::string &val) {test.m_return = m_default_return = std::stoi(val);}},
        {"timeout", [&](const std::string &val) {test.m_timeout = m_default_timeout = std::stof(val);}},
    };
    std::map<std::string_view, std::function<void(const std::string &)>> testParamMap {
        {"@stdout", [&](const std::string &val) {test.m_stdout = (val == "true");}},
        {"@stderr", [&](const std::string &val) {test.m_stderr = (val == "true");}},
        {"@return", [&](const std::string &val) {test.m_return = std::stoi(val);}},
        {"@timeout", [&](const std::string &val) {test.m_timeout = std::stof(val);}},
    };

    while (m_file[m_position].find('[') != 0) {
        auto tab = Utils::stringToVector(m_file[m_position], ' ');
        if (m_file[m_position].find("@default") == 0)
            defaultParamMap[tab[1]](tab[2]);
        else if (m_file[m_position].find('@') == 0)
            testParamMap[tab[0]](tab[1]);
        m_position += 1;
    }
    test.m_name = m_file[m_position].substr(1, m_file[m_position].find(']') - 1);
    test.m_cmd = m_file[m_position].substr(m_file[m_position].find(']') + 1);
    m_position++;
    for (; m_file[m_position] != "[END]"; m_position++)
        test.m_output += m_file[m_position] + '\n';
    m_position++;
    if (!test.m_output.empty())
        test.m_output.pop_back();
    return test;
}