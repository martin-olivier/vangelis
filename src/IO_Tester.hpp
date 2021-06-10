#pragma once

#include <string>
#include <vector>
#include <exception>

#define VERSION "1.7.2"

#ifdef __APPLE__
#define VSCodePath "\"/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code\" --diff "
#else
#define VSCodePath "code --diff "
#endif

class Test
{
public:
    enum Status {PASS, CRASH, FAILED, TIMEOUT, ERROR, NIL};

    std::string m_name{};
    std::string m_cmd{};
    std::string m_output{};
    int m_return_value = 0;
    Status m_status = NIL;
    Test() = default;
    ~Test() = default;
};

class IOTester
{
public:
    enum Details {NO, DETAILS, DIFF};

    class exception : public std::exception
    {
    protected:
        const std::string m_error;
    public:
        explicit exception(std::string message) : m_error(std::move(message)) {};
        [[nodiscard]] const char *what() const noexcept override {return m_error.c_str();};
    };

    IOTester(int ac, char **av);
    ~IOTester() = default;

    Test getTestData();
    void comparator(const Test &test);
    void printFinalResults() const noexcept;
    void apply();
    void resetValues() noexcept;

    static void Version() noexcept;
    static void Help(const char *bin, int returnValue) noexcept;
    static void VSCodeDiff(const Test &test, const std::string &output);
    static bool CheckVSCodeBin();
    static void CheckUpdate() noexcept;
    static void Update() noexcept;
    static void Changelog() noexcept;

    static void display(Test test, const std::string &output, int returnValue, Details details);
    static void compute(const Test &test, pid_t pid, int &status, Details details);

    [[nodiscard]] inline bool exitStatus() const noexcept {return m_return;};
private:
    int m_passed = 0;
    int m_failed = 0;
    int m_crashed = 0;
    int m_timeout = 0;
    size_t m_position = 0;
    Details m_details = NO;
    bool m_return = EXIT_SUCCESS;
    float m_timeout_value = 3.0;
    std::vector<std::string> m_file{};
};