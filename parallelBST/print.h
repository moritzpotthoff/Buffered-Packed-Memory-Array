#pragma once

#include <algorithm>
#include <iostream>
#include <iterator>
#include <mutex>
#include <utility>

static std::mutex GlobalCoutMutex;

#define VERBOSE

#ifdef VERBOSE

#define PRINT_BEGIN(msg) std::cout << ">\t" << (msg) << std::endl;
#define PRINT(msg, value) { \
        std::lock_guard<std::mutex> lock(GlobalCoutMutex); \
        std::cout << (msg) << "\t" << (value) << std::endl;}

#define PRINT1(msg) std::cout << (msg) << std::endl;
#define PRINT_END std::cout << std::endl;

#else

#define PRINT_BEGIN(msg)
#define PRINT(msg, value)
#define PRINT_END

#endif
namespace std
{
    static inline std::ostream& operator<<(std::ostream& out, uint8_t val)
    {
        return out << static_cast<uint32_t>(val);
    }

    template<typename T, typename U>
    static inline std::ostream& operator<<(std::ostream& out, const std::pair<T, U>& pair)
    {
        return out << pair.first << '\t' << pair.second;
    }

    template<typename T>
    static inline std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec)
    {
        std::copy(vec.begin(), vec.end(), std::ostream_iterator<T>(out, " | "));
        return out;
    }
}