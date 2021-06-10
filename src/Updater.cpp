#include "IO_Tester.hpp"
#include "Utils.hpp"
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <string_view>

void IOTester::Changelog()
{
    constexpr std::string_view changelog[] = {
            "(1.0)",
            "> [ADD] IO_Tester",
            "(1.1)",
            "> [FIX] Segmentation Fault now appears properly",
            "(1.2)",
            "> [ADD] VS Code diff (--diff)",
            "(1.3)",
            "> [ADD] Updater (-u || --update)",
            "> [ADD] Changelog (-c || --changelog)",
            "(1.4)",
            "> [FIX] Optimised Updater",
            "> [ADD] return value (0 when all tests succeed, 1 otherwise)",
            "(1.5)",
            "> [ADD] You can now put comments between tests in test files",
            "> [FIX] Parsing Errors",
            "> [FIX] Updater now only works when IO_Tester is already installed on the computer",
            "(1.6)",
            "> [ADD] stderr is now catch like stdout",
            "> [FIX] Parsing Errors",
            "(1.6.1)",
            "> [FIX] Optimised file loading",
            "(1.6.2)",
            "> [ADD] Now builds with C++17",
            "> [FIX] Minor Details",
            "(1.7.0)",
            "> [ADD] Timeout",
            "> [ADD] Expected Return Value",
            "(1.7.1)",
            "> [ADD] Timeout value can be a float",
            "> [FIX] Better error messages",
    };
    std::cout << "[CHANGELOG] :" << std::endl;
    for (auto line : changelog)
        std::cout << line << std::endl;
    exit(0);
}

void IOTester::CheckUpdate()
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
        std::cout << std::endl << MAG << "[UPDATE]" << RESET << " > New Version Available > sudo IO_Tester --update" << std::endl;
}

void IOTester::Update()
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