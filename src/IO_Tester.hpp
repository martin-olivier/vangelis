#ifndef IO_TESTER_H
#define IO_TESTER_H

#include "ErrorHandling.hpp"
#include "Utils.hpp"
#include <string>
#include <vector>

#define VERSION "1.3"

#ifdef __APPLE__
#define VSCodePath "\"/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code\" --diff "
#else
#define VSCodePath "code --diff "
#endif

class Test
{
public:
    enum Status {PASS, CRASH, FAILED};

    std::string m_name;
    std::string m_cmd;
    std::string m_output;
    Status m_status;
    Test() {m_status = FAILED;};
    ~Test() = default;
};

class IOTester
{
public:
    enum Details {NO, DETAILS, DIFF};
    enum VSCodeBin {KO, OK, UNCHECKED};

    static void Version();
    static void VSCodeDiff(const Test &t, const Utils::CMD &c);
    IOTester(int ac, char **av);
    ~IOTester() = default;
    Test getTestData();
    void comparator(Test t);
    void printFinalResults() const;
    void apply();
    void resetValues();

    void checkVSCodeBin();

    static void CheckUpdate();
    static void Update();
    static void Changelog();
private:
    int m_passed;
    int m_failed;
    int m_crashed;
    size_t m_position;
    Details m_details;
    VSCodeBin m_VSCodeBin;
    std::vector<std::string> m_file;
};

#endif