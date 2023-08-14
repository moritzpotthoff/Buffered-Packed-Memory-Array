/**
 * Inspired by Konrad Rudolph: https://stackoverflow.com/questions/3767869/adding-message-to-assert/3767883#3767883
 * 01/09/2023
 */

#ifndef NDEBUG
#include <iostream>

#   define AssertMsg(cond, msg) \
    do { \
        if (! (cond)) { \
            std::cerr << "\033[31m" \
                      << "Assertion failed: " #cond << std::endl \
                      << "    with message: '" << msg << "'" << std::endl \
                      << "         in file: '" << __FILE__ << "'" << std::endl \
                      << "            line: " << __LINE__ << std::endl \
                      << "\033[0m"; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define AssertMsg(cond, msg) do { } while (false)
#endif