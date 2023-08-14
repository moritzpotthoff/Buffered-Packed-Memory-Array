#pragma once

#include <chrono>
#include <random>
#include <thread>

struct TMarsagliaRand
{
private:
    const static unsigned long Seed = 2463534242;
    unsigned long RndVal;

public:
    TMarsagliaRand()
            :   RndVal(Seed)
    {}

    unsigned long operator() ()
    {
        RndVal ^= (RndVal << 13);
        RndVal = (RndVal >> 17);
        return RndVal ^= (RndVal << 5);
    }

    // [left, right]
    unsigned long operator() (size_t left, size_t right)
    {
        return left + (*this)() % (right - left + 1);
    }

    void Reset()
    {
        RndVal = Seed;
    }
};

template <typename T>
class TThreadRandom
{
private:
    std::mt19937 Rng;
    std::uniform_int_distribution<T> Rnd;

public:
    explicit TThreadRandom(T seed)
    {
        auto initSeed = seed;
        Rng.seed(initSeed);
    }

    explicit TThreadRandom()
    {
        auto initSeed = std::hash<std::thread::id>()(std::this_thread::get_id());
        Rng.seed(initSeed);
    }

    T operator() ()
    {
        return Rnd(Rng);
    }

    // [begin, end)
    T operator() (T begin, T end)
    {
        return begin + Rnd(Rng) % (end - begin);
    }
};
