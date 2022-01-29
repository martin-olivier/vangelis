#include <iostream>
#include <filesystem>
#include <unistd.h>

#include "io_tester.hpp"
#include "format.hpp"

void io_tester::changelog()
{
    const std::string changelog[] = {
            std::string(format::blue) + "(1.0)" + format::reset,
            "> [ADD] IO_Tester",
            std::string(format::blue) + "(1.1)" + format::reset,
            "> [FIX] segmentation fault now appears properly",
            std::string(format::blue) + "(1.2)" + format::reset,
            "> [ADD] Visual Studio Code diff (--diff)",
            std::string(format::blue) + "(1.3)" + format::reset,
            "> [ADD] updater (-u || --update)",
            "> [ADD] changelog (-c || --changelog)",
            std::string(format::blue) + "(1.4)" + format::reset,
            "> [FIX] optimised updater",
            "> [ADD] return value (0 when all tests succeed, 1 otherwise)",
            std::string(format::blue) + "(1.5)" + format::reset,
            "> [ADD] comments between tests in test files",
            "> [FIX] parsing errors",
            "> [FIX] updater now only works when IO_Tester is installed",
            std::string(format::blue) + "(1.6)" + format::reset,
            "> [ADD] stderr is now catch like stdout",
            "> [FIX] parsing errors",
            std::string(format::blue) + "(1.6.1)" + format::reset,
            "> [FIX] optimised file loading",
            std::string(format::blue) + "(1.6.2)" + format::reset,
            "> [ADD] now builds with C++17",
            "> [FIX] minor details",
            std::string(format::blue) + "(1.7.0)" + format::reset,
            "> [ADD] timeout",
            "> [ADD] expected return value",
            std::string(format::blue) + "(1.7.1)" + format::reset,
            "> [ADD] timeout value can be a float",
            "> [FIX] better error messages",
            std::string(format::blue) + "(1.7.2)" + format::reset,
            "> [ADD] better error messages",
            "> [ADD] throw and noexcept",
            "> [REM] removed '>' and '->' before return values",
            std::string(format::blue) + "(1.8.0)" + format::reset,
            "> [ADD] test parameters",
            "> [ADD] parameter: stdout",
            "> [ADD] parameter: stderr",
            "> [ADD] parameter: return",
            "> [ADD] parameter: timeout",
            "> [ADD] details and diff limit",
            "> [FIX] args parser",
            "> [FIX] cleaner display",
            std::string(format::blue) + "(1.8.1)" + format::reset,
            "> [FIX] the test pid is killed only if process is not terminated",
            "> [FIX] no useless newline in --diff display",
            "> [FIX] missing display if --diff and Visual Studio Code not installed",
            "> [FIX] code is now in camelCase",
            "> [FIX] args parser",
            std::string(format::blue) + "(1.8.2)" + format::reset,
            "> [FIX] updater",
            "> [FIX] the test pid is not threaded anymore",
            std::string(format::blue) + "(1.9.0)" + format::reset,
            "> [ADD] New Design and cleaner output",
            "> [ADD] CMake build",
            "> [FIX] Cleaner documentation",
            "> [FIX] Cleaner code",
    };
    std::cout << format::yellow << "[CHANGELOG]" << + format::reset << std::endl;
    for (const auto &line : changelog)
        std::cout << line << std::endl;
    exit(0);
}

void io_tester::check_update()
{
    if (access("/usr/local/bin/IO_Tester", X_OK) == -1)
        return;
    if (std::filesystem::is_directory("/tmp/IO-TESTER")) {
        if (system("git --help > /dev/null 2>&1") != 0)
            return;
        system("git clone https://github.com/martin-olivier/IO-TESTER.git /tmp/IO-TESTER > /dev/null 2>&1 &");
        return;
    }
    if (access("/tmp/IO-TESTER/IO_Tester", X_OK) == -1) {
        system("make -C /tmp/IO-TESTER > /dev/null 2>&1 &");
        return;
    }
    if (system("diff /tmp/IO-TESTER/IO_Tester /usr/local/bin/IO_Tester > /dev/null 2>&1") != 0)
        std::cout << std::endl << format::magenta << "[UPDATE]" << format::reset << " A new version is available : sudo IO_Tester --update" << std::endl;
}

void io_tester::update()
{
    if (access("/usr/local/bin/IO_Tester", X_OK) == -1) {
        std::cerr << format::red << "[X] " << format::reset << "IO_Tester needs to be installed to be updated (sudo make install)" << std::endl;
        exit(84);
    }
    if (system("git --help > /dev/null 2>&1") != 0) {
        std::cerr << format::red << "[X] " << format::reset << "Git is not installed, could not update\n" << std::endl;
        exit(84);
    }
    if (access("/usr/local/bin", W_OK) == -1) {
        std::cerr << format::red << "[X] " << format::reset << "Insuffisant permissions, re-run with sudo" << std::endl;
        exit(84);
    }
    if (!std::filesystem::is_directory("/tmp/IO-TESTER")) {
        if (system("git clone https://github.com/martin-olivier/IO-TESTER.git /tmp/IO-TESTER > /dev/null 2>&1") != 0) {
            std::cerr << format::red << "[X] " << format::reset << "Download failed, check your internet connection" << std::endl;
            exit(84);
        }
    }

    if (access("/tmp/IO-TESTER/IO_Tester", X_OK) == -1) {
        if (system("make -C /tmp/IO-TESTER > /dev/null 2>&1") != 0) {
            std::cerr << format::red << "[X] " << format::reset << "Build failed" << std::endl;
            exit(84);
        }
    }
    if (system("diff /tmp/IO-TESTER/IO_Tester /usr/local/bin/IO_Tester > /dev/null 2>&1") == 0) {
        std::cout << format::green << "[O] " << format::reset << "Already up-to-date v." << IO_TESTER_VERSION << std::endl;
        exit(0);
    }

    constexpr const char *args[] = {"cp", "/tmp/IO-TESTER/IO_Tester", "/usr/local/bin", nullptr};

    std::cout << format::green << "[O] " << format::reset << "IO_Tester has been updated! run IO_Tester -c to see changelog" << std::endl;
    execvp("cp", const_cast<char *const *>(args));
    exit(0);
}