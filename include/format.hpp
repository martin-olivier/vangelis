#pragma once

/**
 *  This namespace contains every text format that you can apply on the shell
 */
namespace format {
    constexpr auto red     =  "\x1B[31m";
    constexpr auto green   =  "\x1B[32m";
    constexpr auto yellow  =  "\x1B[33m";
    constexpr auto blue    =  "\x1B[34m";
    constexpr auto magenta =  "\x1B[35m";
    constexpr auto cyan    =  "\x1B[36m";
    constexpr auto bold    =  "\033[1m" ;
    constexpr auto reset   =  "\x1B[0m" ;
}