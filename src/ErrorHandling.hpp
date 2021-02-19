#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H

#include <iostream>
#include <vector>

class ErrorHandling
{
protected:
    bool m_details;
    std::vector<std::string> m_file;
public:
    ErrorHandling(int ac, char **av);
    ~ErrorHandling() = default;
    void CheckLoop();
    static void isInput(const std::string &line);
};

#endif