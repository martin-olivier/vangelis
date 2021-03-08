#include "IO_Tester.hpp"
#include <unistd.h>

void IOTester::VSCodeDiff(const Test &t, const Utils::CMD &c)
{
    std::string filename1 = "\"/tmp/GOT(" + t.m_name + ")\"";
    std::string filename2 = "\"/tmp/EXPECTED(" + t.m_name + ")\"";
    std::string s1 = "echo \"" + c.output + "\" > " + filename1;
    std::string s2 = "echo \"" + t.m_output + "\" > " + filename2;

    system(std::string(s1 + " ; " + s2).c_str());
    system(std::string(VSCodePath + filename1 + " " + filename2).c_str());
}

void IOTester::checkVSCodeBin()
{
#ifdef __APPLE__
    if (access((std::string("/Applications/Visual Studio Code.app/Contents/Resources/app/bin/code").c_str()), X_OK) != -1) {
        m_VSCodeBin = OK;
        return;
    }
    m_VSCodeBin = KO;
#else
    char* env_p = std::getenv("PATH");
    if (env_p == NULL) {
        m_VSCodeBin = KO;
        return;
    }
    auto PATHList = Utils::string_to_vector(env_p, ':');

    for (auto &path : PATHList) {
        if (access((std::string(path + "/code").c_str()), X_OK) != -1) {
            m_VSCodeBin = OK;
            return;
        }
    }
    m_VSCodeBin = KO;
#endif
}