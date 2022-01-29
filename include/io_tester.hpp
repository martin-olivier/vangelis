#pragma once

#include <string>
#include <vector>
#include <exception>
#include <string_view>

constexpr auto IO_TESTER_VERSION = "1.9.0";

class test final
{
public:
    enum status {nil, passed, crashed, failed, timeout, error};

    test(bool a_stdout, bool a_stderr, int a_return, float a_timeout)
        : m_stdout(a_stdout), m_stderr(a_stderr), m_return(a_return), m_timeout(a_timeout) {}
    ~test() = default;
    std::string m_name{};
    std::string m_cmd{};
    std::string m_output{};
    status m_status = nil;
    bool m_stdout;
    bool m_stderr;
    int m_return;
    float m_timeout;
};

class io_tester final
{
public:
    enum details {no, shell, vsdiff};

    class exception : public std::exception
    {
    protected:
        const std::string m_error;
    public:
        explicit exception(std::string message) : m_error(std::move(message)) {}
        [[nodiscard]] inline const char *what() const noexcept override {return m_error.c_str();}
    };

    io_tester(int ac, char **av);
    ~io_tester() = default;

    std::vector<std::string_view> parse_args(int ac, char **av);
    test get_test_data();
    void comparator(const test &test);
    void print_results() const noexcept;
    void apply();
    void reset_values() noexcept;
    [[nodiscard]] inline bool exit_status() const noexcept {return m_return;}

    static void version() noexcept;
    static void help(const char *bin, int return_value) noexcept;
    static void vscode_diff(const test &test, const std::string &output);
    static bool check_vscode_bin();
    static void check_update();
    static void update();
    static void changelog();

    static void display(test test, const std::string &output, int returnValue, details details);
    static void compute(const test &test, pid_t pid, int &status, details details);
    static std::string get_command(const test &test);

    bool m_default_stdout = true;
    bool m_default_stderr = true;
    int m_default_return = 0;
    float m_default_timeout = 3.0;

private:
    int m_passed = 0;
    int m_failed = 0;
    int m_crashed = 0;
    int m_timeout = 0;
    std::vector<std::string> m_file{};
    size_t m_position = 0;
    details m_details = no;
    size_t m_details_count = -1;
    bool m_return = EXIT_SUCCESS;
};