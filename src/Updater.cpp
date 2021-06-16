#include "IO_Tester.hpp"
#include "Utils.hpp"
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string_view>

void IOTester::Changelog() noexcept
{
    constexpr std::string_view changelog[] = {
            BLU "(1.0)" RESET,
            "> [ADD] IO_Tester",
            BLU "(1.1)" RESET,
            "> [FIX] Segmentation fault now appears properly",
            BLU "(1.2)" RESET,
            "> [ADD] Visual Studio Code diff (--diff)",
            BLU "(1.3)" RESET,
            "> [ADD] Updater (-u || --update)",
            "> [ADD] Changelog (-c || --changelog)",
            BLU "(1.4)" RESET,
            "> [FIX] Optimised Updater",
            "> [ADD] return value (0 when all tests succeed, 1 otherwise)",
            BLU "(1.5)" RESET,
            "> [ADD] Comments between tests in test files",
            "> [FIX] Parsing errors",
            "> [FIX] Updater now only works when IO_Tester is installed",
            BLU "(1.6)" RESET,
            "> [ADD] stderr is now catch like stdout",
            "> [FIX] Parsing errors",
            BLU "(1.6.1)" RESET,
            "> [FIX] Optimised file loading",
            BLU "(1.6.2)" RESET,
            "> [ADD] Now builds with C++17",
            "> [FIX] Minor details",
            BLU "(1.7.0)" RESET,
            "> [ADD] Timeout",
            "> [ADD] Expected return value",
            BLU "(1.7.1)" RESET,
            "> [ADD] Timeout value can be a float",
            "> [FIX] Better error messages",
            BLU "(1.7.2)" RESET,
            "> [ADD] Better error messages",
            "> [ADD] throw and noexcept",
            "> [REM] Removed '>' and '->' before return values",
    };
    std::cout << YEL << "[CHANGELOG]" << RESET << std::endl;
    for (auto line : changelog)
        std::cout << line << std::endl;
    exit(0);
}

void IOTester::CheckUpdate() noexcept
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
        std::cout << std::endl << MAG << "[UPDATE]" << RESET << " > A new version is available : sudo IO_Tester --update" << std::endl;
}

void IOTester::Update() noexcept
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

    constexpr char *args[] = {(char *)"cp", (char *)"/tmp/IO-TESTER/IO_Tester", (char *)"/usr/local/bin", NULL};

    std::cout << GRN << "[SUCCESS] Install" << RESET << " > run IO_Tester -c to see changelog" << std::endl;
    if (execvp("cp", args) != 0) {
        std::cerr << RED << "[FAILED] Install" << RESET << std::endl;
        exit(84);
    }
    exit(0);
}