#include <iostream>
#include "IO_Tester.hpp"
#include "format.hpp"

int main(int ac, char **av)
{
    try {
        IOTester app(ac, av);
        return app.exitStatus();
    }
    catch (const std::exception &e) {
        std::cerr << format::red << "error: " << format::reset << e.what() << std::endl;
        return 84;
    }
    catch (...) {
        std::cerr << format::red << "error: " << format::reset << "unknown exception caught" << std::endl;
        return 84;
    }
}