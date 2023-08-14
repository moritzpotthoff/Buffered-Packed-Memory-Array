#pragma once

#include <mutex>
#include <set>

#include "profiler.hpp"
#include "debugStream.hpp"
#include "typedefs.hpp"

namespace DataStructures {

template <typename T, typename PROFILER = Helpers::Profiler::NoProfiler, typename DEBUG_STREAM = Helpers::NoDebugStream<T>>
class SimpleOrderedSet {
public:
    using ValueType = T;
    using Reference = T&;
    using ConstReference = T const&;

    using Profiler = PROFILER;
    using DebugStream = DEBUG_STREAM;

private:
    std::mutex m;
    std::set<T> data;

public:
    Profiler profiler;
    DebugStream debugStream;

public:
    void insert(ConstReference e) {
        auto l = std::scoped_lock(m);
        data.insert(e);
    }

    void remove(ConstReference e) {
        auto l = std::scoped_lock(m);
        data.erase(e);
    }

    bool search(ConstReference e) {
        auto l = std::scoped_lock(m);
        return data.contains(e);
    }

    size_t size() {
        return data.size();
    }

    std::unordered_set<T>::iterator begin() {
        return data.begin();
    }
    std::unordered_set<T>::iterator end() {
        return data.end();
    }

public:
    static const std::string type() {
        return "std::set";
    }

    void printContent() {}
    bool isValid() { return true; }
};
}
