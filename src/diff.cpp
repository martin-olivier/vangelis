#include <unistd.h>

#include "io_tester.hpp"
#include "tools.hpp"

#ifdef __APPLE__
const std::string vscode_path = "/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code";
#else
const std::string vscode_path = "code";
#endif

void io_tester::vscode_diff(const test &test, const std::string &output)
{
    const std::string filename1 = "\"/tmp/GOT(" + test.m_name + ")\"";
    const std::string filename2 = "\"/tmp/EXPECTED(" + test.m_name + ")\"";
    const std::string s1 = "printf \"" + output + "\" > " + filename1;
    const std::string s2 = "printf \"" + test.m_output + "\" > " + filename2;

    system(std::string(s1 + " ; " + s2).c_str());
    system(std::string("\"" + vscode_path + "\" --diff " + filename1 + " " + filename2).c_str());
}

bool io_tester::check_vscode_bin()
{
#ifdef __APPLE__
    if (access(vscode_path.c_str(), X_OK) != -1)
        return true;
    return false;
#else
    char* env_p = std::getenv("PATH");
    if (env_p == NULL)
        return false;
    auto path_list = tools::string_to_vector(env_p, ':');

    for (auto &path : path_list) {
        if (access((std::string(path + "/code").c_str()), X_OK) != -1)
            return true;
    }
    return false;
#endif
}