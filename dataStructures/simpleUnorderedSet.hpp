#pragma once

#include <mutex>
#include <unordered_set>

#include "profiler.hpp"
#include "debugStream.hpp"
#include "typedefs.hpp"

namespace DataStructures {

template <typename T, typename PROFILER = Helpers::Profiler::NoProfiler, typename DEBUG_STREAM = Helpers::NoDebugStream<T>>
class SimpleUnorderedSet {
public:
    using ValueType = T;
    using Reference = T&;
    using ConstReference = T const&;

    using Profiler = PROFILER;
    using DebugStream = DEBUG_STREAM;

public:
    Profiler profiler;
    DebugStream debugStream;

private:
    std::mutex m;
    std::unordered_set<T> data;

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
        return data.find(e) != data.end();
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

    void insertBatch(T *batch, int64_t batchSize) {
        for (int i = 0; i < batchSize; ++i) insert(batch[i]);
    }

    void removeBatch(T *batch, int64_t batchSize) {
        for (int i = 0; i < batchSize; ++i) remove(batch[i]);
    }

    int64_t numberOfElements() {
        return data.size();
    }

    template<typename EVALUATOR>
    void rangeQuery(T left, T right, EVALUATOR f) const {
        for (const T &el : data) {
            if (el >= left && el < right) {
                f(el);
            }
        }
    }

public:
    static const std::string type() {
        return "std::unordered_set";
    }
    void printContent() {}
    bool isValid() { return true; }
};
}
