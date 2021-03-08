#include "IO_Tester.hpp"
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

void IOTester::Changelog()
{
    constexpr static const char* const changelog[] = {
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
            NULL
    };
    std::cout << "[CHANGELOG] :" << std::endl;
    for (size_t i = 0; changelog[i] != NULL; i++)
        std::cout << changelog[i] << std::endl;
    exit(0);
}

void IOTester::CheckUpdate()
{
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

    constexpr char *const args[] = {(char *)"cp", (char *)"/tmp/IO-TESTER/IO_Tester", (char *)"/usr/local/bin", NULL};

    std::cout << GRN << "[SUCCESS] Install" << RESET << " > " << BLU << "run IO_Tester -c to see changelog" << std::endl;
    if (execvp("cp", args) != 0) {
        std::cerr << RED << "[FAILED] Install" << RESET << std::endl;
        exit(84);
    }
    exit(0);
}