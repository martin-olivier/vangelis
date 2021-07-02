#include "IO_Tester.hpp"
#include "Utils.hpp"
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string_view>
#include <sys/wait.h>

void IOTester::changelog() noexcept
{
    constexpr std::string_view changelog[] = {
            BLU "(1.0)" RESET,
            "> [ADD] IO_Tester",
            BLU "(1.1)" RESET,
            "> [FIX] segmentation fault now appears properly",
            BLU "(1.2)" RESET,
            "> [ADD] Visual Studio Code diff (--diff)",
            BLU "(1.3)" RESET,
            "> [ADD] updater (-u || --update)",
            "> [ADD] changelog (-c || --changelog)",
            BLU "(1.4)" RESET,
            "> [FIX] optimised updater",
            "> [ADD] return value (0 when all tests succeed, 1 otherwise)",
            BLU "(1.5)" RESET,
            "> [ADD] comments between tests in test files",
            "> [FIX] parsing errors",
            "> [FIX] updater now only works when IO_Tester is installed",
            BLU "(1.6)" RESET,
            "> [ADD] stderr is now catch like stdout",
            "> [FIX] parsing errors",
            BLU "(1.6.1)" RESET,
            "> [FIX] optimised file loading",
            BLU "(1.6.2)" RESET,
            "> [ADD] now builds with C++17",
            "> [FIX] minor details",
            BLU "(1.7.0)" RESET,
            "> [ADD] timeout",
            "> [ADD] expected return value",
            BLU "(1.7.1)" RESET,
            "> [ADD] timeout value can be a float",
            "> [FIX] better error messages",
            BLU "(1.7.2)" RESET,
            "> [ADD] better error messages",
            "> [ADD] throw and noexcept",
            "> [REM] removed '>' and '->' before return values",
            BLU "(1.8.0)" RESET,
            "> [ADD] test parameters",
            "> [ADD] parameter: stdout",
            "> [ADD] parameter: stderr",
            "> [ADD] parameter: return",
            "> [ADD] parameter: timeout",
            "> [ADD] details and diff limit",
            "> [FIX] args parser",
            "> [FIX] cleaner display",
            BLU "(1.8.1)" RESET,
            "> [FIX] the test pid is killed only if process is not terminated",
            "> [FIX] missing display if --diff and Visual Studio Code not installed",
            "> [FIX] code is now in camelCase",
            "> [FIX] args parser",
    };
    std::cout << YEL << "[CHANGELOG]" << RESET << std::endl;
    for (auto line : changelog)
        std::cout << line << std::endl;
    exit(0);
}

void IOTester::checkUpdate() noexcept
{
    if (access("/usr/local/bin/IO_Tester", X_OK) == -1)
        return;
    DIR *dir = opendir("/tmp/IO-TESTER");
    if (!dir) {
        if (system("git --help > /dev/null 2>&1") != 0)
            return;
        system("git clone https://github.com/tocola/IO-TESTER.git /tmp/IO-TESTER > /dev/null 2>&1 &");
        return;
    }
    closedir(dir);
    if (access("/tmp/IO-TESTER/IO_Tester", X_OK) == -1) {
        system("make -C /tmp/IO-TESTER > /dev/null 2>&1 &");
        return;
    }
    if (system("diff /tmp/IO-TESTER/IO_Tester /usr/local/bin/IO_Tester > /dev/null 2>&1") != 0)
        std::cout << std::endl << MAG << "[UPDATE]" << RESET << " A new version is available : sudo IO_Tester --update" << std::endl;
}

void IOTester::update() noexcept
{
    if (access("/usr/local/bin/IO_Tester", X_OK) == -1) {
        std::cerr << RED << "\nIO_Tester needs to be installed to be updated (sudo make install)\n" << RESET << std::endl;
        exit(84);
    }
    if (system("git --help > /dev/null 2>&1") != 0) {
        std::cerr << RED << "\nYou need to install git to update\n" << RESET << std::endl;
        exit(84);
    }
    if (access("/usr/local/bin", W_OK) == -1) {
        std::cout << RED << "[FAILED] re-run with sudo" << RESET << std::endl;
        exit(84);
    }
    DIR *dir = opendir("/tmp/IO-TESTER");
    if (!dir) {
        if (system("git clone https://github.com/tocola/IO-TESTER.git /tmp/IO-TESTER > /dev/null 2>&1") != 0) {
            std::cout << RED << "[FAILED] Download" << RESET << std::endl;
            exit(84);
        }
    }
    if (dir)
        closedir(dir);

    if (access("/tmp/IO-TESTER/IO_Tester", X_OK) == -1) {
        if (system("make -C /tmp/IO-TESTER > /dev/null 2>&1") != 0) {
            std::cerr << RED << "\n[FAILED] Build\n" << RESET << std::endl;
            exit(84);
        }
    }
    if (system("diff /tmp/IO-TESTER/IO_Tester /usr/local/bin/IO_Tester > /dev/null 2>&1") == 0) {
        std::cout << "Already up-to-date : (" << VERSION << ")" << std::endl;
        exit(0);
    }

    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << RED << "[FAILED] Install" << RESET << std::endl;
        exit(84);
    } else if (pid == 0) {
        constexpr char *args[] = {(char *)"cp", (char *)"/tmp/IO-TESTER/IO_Tester", (char *)"/usr/local/bin", nullptr};
        if (execvp("cp", args) != 0) {
            std::cerr << RED << "[FAILED] Install" << RESET << std::endl;
            exit(84);
        }
    } else {
        int ret = 0;
        waitpid(pid, &ret, WUNTRACED | WCONTINUED);
        if (ret != 0) {
            std::cerr << RED << "[FAILED] Install" << RESET << std::endl;
            exit(84);
        }
        std::cout << GRN << "[SUCCESS] Install" << RESET << " > run IO_Tester -c to see changelog" << std::endl;
    }
    exit(0);
}