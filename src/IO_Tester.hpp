#pragma once

#include "ErrorHandling.hpp"
#include "Utils.hpp"
#include <string>
#include <vector>

#define VERSION "1.7.1"

#ifdef __APPLE__
#define VSCodePath "\"/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code\" --diff "
#else
#define VSCodePath "code --diff "
#endif

class Test
{
public:
    enum Status {PASS, CRASH, FAILED, TIMEOUT, ERROR, NIL};

    std::string m_name;
    std::string m_cmd;
    std::string m_output;
    int m_return_value;
    Status m_status;
    Test() {m_status = NIL;};
    ~Test() = default;
};

class IOTester
{
public:
    enum Details {NO, DETAILS, DIFF};

    static void Version();
    static void VSCodeDiff(const Test &t, const std::string &output);
    IOTester(int ac, char **av);
    ~IOTester() = default;
    Test getTestData();
    void comparator(Test t);
    void printFinalResults() const;
    void apply();
    void resetValues();

    static bool checkVSCodeBin();

    static void CheckUpdate();
    static void Update();
    static void Changelog();

    bool exitStatus() const {return m_return;};
private:
    int m_passed;
    int m_failed;
    int m_crashed;
    int m_timeout;
    size_t m_position;
    Details m_details;
    bool m_return;
    float m_timeout_value;
    std::vector<std::string> m_file;
};