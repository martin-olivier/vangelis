#include <iostream>

#include "io_tester.hpp"
#include "format.hpp"

int main(int ac, char **av)
{
    try {
        io_tester app(ac, av);
        return app.exit_status();
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