#pragma once

#include <iostream>
#include <iomanip>
#include <vector>
#include <thread>
#include <map>
#include <cstring>
#include <mutex>
#include <cstdint>

#include "timer.hpp"
#include "assert.hpp"
#include "configuration.hpp"

namespace Helpers::Profiler {
static constexpr size_t NumberOfThreadsPrinted = 128;

enum Phase {
    INSERTION_DISTRIBUTION,
    INSERTION_EXECUTION,
    UPDATE,
    REBALANCING_DISTRIBUTION,
    REBALANCING_EXECUTION,
    phaseCount,//must be the last element
};
constexpr const char* PhaseNames[] = {
    "insertion-distribution",
    "insertion-execution",
    "update",
    "rebalancing-distribution",
    "rebalancing-execution"
};
constexpr int phaseWidth() {
    int maxWidth = 0;
    for (int i = 0; i < phaseCount; ++i) maxWidth = std::max(maxWidth, (int)strlen(PhaseNames[i]));
    return maxWidth;
}

enum ParallelPhase {
    INSERT_DISTRIBUTE_BINARY_SEARCH,
    /*INSERT_DISTRIBUTE_WIDTH,
    INSERT_PARALLEL_SWAP,
    INSERT_PARALLEL_IN_TASK,
    INSERT_PARALLEL_OUT_TASK,
    INSERT_BATCH_IN_FIND_BATCH_END,
    INSERT_BATCH_IN_BLOCK_NO_SPLIT,
    INSERT_BATCH_IN_BLOCK_SPLIT,
    INSERT_BATCH_IN_ITERATE_BLOCKS,
    INSERT_BATCH_IN_ITERATE_BLOCKS_EXP,
    INSERT_BATCH_IN_ITERATE_BLOCKS_BIN,
    INSERT_BATCH_IN_ITERATE_BLOCKS_LIN,
    INSERT_BATCH_IN_INSERT_ELEMENTS,
    INSERT_BATCH_IN_MERGE_NORMAL,
    INSERT_BATCH_IN_MERGE_FASTER,
    INSERT_BATCH_OUT_FIND_BATCH_END,
    INSERT_BATCH_OUT_WRITE_BLOCK_NO_INSERT,
    INSERT_BATCH_OUT_WRITE_BLOCK_NO_SPLIT,
    INSERT_BATCH_OUT_WRITE_BLOCK_SPLIT,
    INSERT_BATCH_OUT_ITERATE_BLOCKS,
    INSERT_BATCH_OUT_INSERT_ELEMENTS,*/
    parallelPhaseCount,//must be the last element
};
constexpr const char* ParallelPhaseNames[] = {
        "insert-distribute-binarySearch",
        /*"insert-distribute-getNumberOfElements",
        "insert-swapParallel",
        "insert-parallelIn-executeTask",
        "insert-parallelOut-executeTask",
        "insert-parallelIn-findBatchEnd",
        "insert-parallelIn-blockWithoutSplit",
        "insert-parallelIn-blockWithSplit",
        "insert-parallelIn-iterateBlocks",
        "insert-parallelIn-iterateBlocksExp",
        "insert-parallelIn-iterateBlocksBin",
        "insert-parallelIn-iterateBlocksLin",
        "insert-parallelIn-insertElements",
        "insert-parallelIn-mergeNormal",
        "insert-parallelIn-mergeFaster",
        "insert-parallelOut-findBatchEnd",
        "insert-parallelOut-writeBlockWithoutInsert",
        "insert-parallelOut-writeBlockWithoutSplit",
        "insert-parallelOut-writeBlocksAfterSplit",
        "insert-parallelOut-iterateOverBlocks",
        "insert-parallelOut-insertElements",*/
};
constexpr int parallelPhaseWidth() {
    int maxWidth = 0;
    for (int i = 0; i < parallelPhaseCount; ++i) maxWidth = std::max(maxWidth, (int)strlen(ParallelPhaseNames[i]));
    return maxWidth;
}

enum Operation {
    SEARCH,
    INSERT,
    REMOVE,
    opCount,//must be the last element
};
constexpr const char* OperationNames[] = {
    "search",
    "insert",
    "remove"
};
constexpr int operationWidth() {
    int maxWidth = 0;
    for (int i = 0; i < opCount; ++i) maxWidth = std::max(maxWidth, (int)strlen(OperationNames[i]));
    return maxWidth;
}

enum Event {
    BATCH_INSERT_IN_PLACE,
    BATCH_INSERT_OUT_OF_PLACE,
    eventCount,//must be the last element
};
constexpr const char* EventNames[] = {
    "batchInsertInPlace",
    "batchInsertOutOfPlace"
};
constexpr int eventWidth() {
    int maxWidth = 0;
    for (int i = 0; i < eventCount; ++i) maxWidth = std::max(maxWidth, (int)strlen(EventNames[i]));
    return maxWidth;
}

static constexpr size_t precision = 3;
static constexpr size_t maxValueWidth = 14 + precision;//At least 15 for header

struct PeriodInformation {
    size_t numberOfStarts;
    size_t numberOfEnds;
    size_t totalTime;
    Timer timer;

    inline void restart() {
        timer.restart();
        ++numberOfStarts;
    }

    inline void end() {
        totalTime += timer.getNanoseconds();
        ++numberOfEnds;
    }

    inline bool wasUsed() const {
        AssertMsg(numberOfStarts == numberOfEnds, "Period was started " << numberOfStarts << " times, but ended " << numberOfEnds << " times.");
        return numberOfEnds > 0;
    }
    PeriodInformation& operator+=(const PeriodInformation &other){
        numberOfStarts += other.numberOfStarts;
        numberOfEnds += other.numberOfEnds;
        totalTime += other.totalTime;
        return *this;
    }

    bool operator<(const PeriodInformation& other) const {
        return totalTime < other.totalTime || numberOfEnds < other.numberOfEnds;
    }

    inline void printResultString() const {
        AssertMsg(numberOfStarts == numberOfEnds, "Period was started " << numberOfStarts << " times, but ended " << numberOfEnds << " times.");
        std::cout   << " totalTime=" << std::fixed << std::setprecision(precision) << ((double)totalTime / 1000.0)
                    << " numberOfCalls=" << numberOfEnds;
        if (numberOfEnds > 0) {
            std::cout   << std::fixed << std::setprecision(precision)
                        << " avgTime=" << ((double)totalTime / (double)numberOfEnds / 1000.0);
        } else {
            std::cout << " avgTime=0";
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const PeriodInformation& period) {
        AssertMsg(period.numberOfStarts == period.numberOfEnds, "Period was started " << period.numberOfStarts << " times, but ended " << period.numberOfEnds << " times.");
        out << std::setw(maxValueWidth) << std::fixed << std::setprecision(precision) << ((double)period.totalTime / 1000.0)
            << std::setw(maxValueWidth) << period.numberOfEnds;
        if (period.numberOfEnds > 0) {
            out << std::fixed << std::setprecision(precision) << std::setw(maxValueWidth) << ((double)period.totalTime / (double)period.numberOfEnds / 1000.0);
        } else {
            out << std::setw(maxValueWidth) << "-";
        }
        return out;
    }
};

class NoProfiler {
public:
    inline void registerThread() const noexcept {}
    inline void reset() {}
    inline void activate() {}
    inline void deactivate() {}
    inline void startPhase(Phase phase) const noexcept { (void)phase; }
    inline void endPhase(Phase phase) const noexcept { (void)phase; }
    inline void startParallelPhase(ParallelPhase phase) const noexcept { (void)phase; }
    inline void endParallelPhase(ParallelPhase phase) const noexcept { (void)phase; }
    inline void startOperation(Operation op) const noexcept { (void)op; }
    inline void endOperation(Operation op) const noexcept { (void)op; }
    inline void countEvent(Event event) const noexcept { (void)event; }
    inline void print() const noexcept {}
    template<typename T>
    inline void printResultLines(Configuration<T> &config, std::string benchmarkIdentifier, std::string sds, int iteration, std::string extra = "") const {
        (void) config;
        (void) benchmarkIdentifier;
        (void) sds;
        (void) iteration;
        (void) extra;
    }


        static inline std::string identifier() {
        return "NoProfiler";
    }
};

class SequentialProfiler {
public:
    std::vector<PeriodInformation> operations;
    std::vector<PeriodInformation> phases;
    std::vector<PeriodInformation> parallelPhases;
    std::vector<std::int64_t> events;

    std::mutex m;
    bool alreadyUsed = false;
    bool active = false;

public:
    SequentialProfiler() :
        operations(opCount),
        phases(phaseCount),
        parallelPhases(parallelPhaseCount),
        events(eventCount) {}

    inline void reset() {
        std::fill(operations.begin(), operations.end(), PeriodInformation());
        std::fill(phases.begin(), phases.end(), PeriodInformation());
        std::fill(parallelPhases.begin(), parallelPhases.end(), PeriodInformation());
        std::fill(events.begin(), events.end(), 0);
    }

    inline void registerThread() {
        auto l = std::scoped_lock(m);
        if (alreadyUsed) {
            //std::cout << "Warning: Using a sequential profiler with more than one thread." << std::endl;
        }
        alreadyUsed = true;
    }

    inline void activate() {
        auto l = std::scoped_lock(m);
        AssertMsg(!active, "Already active.");
        active = true;
    }

    inline void deactivate() {
        auto l = std::scoped_lock(m);
        AssertMsg(active, "Not active.");
        active = false;
    }

    inline void startPhase(Phase phase) {
        if (active) phases[phase].restart();
    }

    inline void endPhase(Phase phase) {
        if (active) phases[phase].end();
    }

    inline void startParallelPhase(ParallelPhase phase) {
        if (active) parallelPhases[phase].restart();
    }

    inline void endParallelPhase(ParallelPhase phase) {
        if (active) parallelPhases[phase].end();
    }

    inline void startOperation(Operation op) {
        if (active) operations[op].restart();
    }

    inline void endOperation(Operation op) {
        if (active) operations[op].end();
    }

    inline void countEvent(Event event) {
        if (active) ++events[event];
    }

    inline void print() const {
        printHeader();
        std::cout << std::endl << std::endl;
        size_t opId = 0;
        for (const PeriodInformation &period : operations) {
            if (period.wasUsed()) {
                std::cout << std::setw(std::max(std::max(phaseWidth(), parallelPhaseWidth()), std::max(operationWidth(), eventWidth()))) << OperationNames[opId]
                    << period << std::endl;
            }
            ++opId;
        }
        std::cout << std::endl;
        size_t phaseId = 0;
        for (const PeriodInformation &period : phases) {
            if (period.wasUsed()) {
                std::cout << std::setw(std::max(std::max(phaseWidth(), parallelPhaseWidth()), std::max(operationWidth(), eventWidth()))) << PhaseNames[phaseId]
                          << period << std::endl;
            }
            ++phaseId;
        }
        std::cout << std::endl;
        size_t parallelPhaseId = 0;
        for (const PeriodInformation &period : parallelPhases) {
            if (period.wasUsed()) {
                std::cout << std::setw(std::max(std::max(phaseWidth(), parallelPhaseWidth()), std::max(operationWidth(), eventWidth()))) << ParallelPhaseNames[parallelPhaseId]
                          << period << std::endl;
            }
            ++parallelPhaseId;
        }
        std::cout << std::endl;
        size_t eventId = 0;
        for (const std::int64_t count : events) {
            if (count > 0) {
                std::cout << std::setw(std::max(std::max(phaseWidth(), parallelPhaseWidth()), std::max(operationWidth(), eventWidth()))) << EventNames[eventId]
                          << std::setw(maxValueWidth) << count << std::endl;
            }
            ++eventId;
        }
    }

    inline void printOperation(size_t id) const {
        std::cout << operations[id];
    }

    inline void printPhase(size_t id) const {
        std::cout << phases[id];
    }

    inline void printParallelPhase(size_t id) const {
        std::cout << parallelPhases[id];
    }

    inline void printEvent(size_t id) const {
        std::cout << std::setw(maxValueWidth) << " " << std::setw(maxValueWidth) << events[id] << std::setw(maxValueWidth) << " ";
    }

    template<typename T>
    inline void printResultLines(Configuration<T> &config, std::string benchmarkIdentifier, std::string sds, int iteration, std::string extra = "", size_t maxOperation = 10000, size_t maxPhase = 10000, size_t maxParallelPhase = 10000, size_t maxEvent = 10000) const {
        size_t opId = 0;
        for (const PeriodInformation &period : operations) {
            if (opId >= maxOperation) break;
            std::cout   << "RESULT benchmark=sequentialProfiler(" << benchmarkIdentifier << ")"
                        << " timeUnit=microseconds"
                        << " type=operation"
                        << " sequenceId=" << opId
                        << " identifier="<< OperationNames[opId]
                        << " sdsType=" << sds
                        << " iteration=" << iteration
                        << " " << extra;
            period.printResultString();
            std::cout << " " << config << std::endl;
            ++opId;
        }
        size_t phaseId = 0;
        for (const PeriodInformation &period : phases) {
            if (phaseId >= maxPhase) break;
            std::cout   << "RESULT benchmark=sequentialProfiler(" << benchmarkIdentifier << ")"
                        << " timeUnit=microseconds"
                        << " type=phase"
                        << " sequenceId=" << phaseId
                        << " identifier="<< PhaseNames[phaseId]
                        << " sdsType=" << sds
                        << " iteration=" << iteration
                        << " " << extra;
            period.printResultString();
            std::cout << " " << config << std::endl;
            ++phaseId;
        }
        size_t parallelPhaseId = 0;
        for (const PeriodInformation &period : parallelPhases) {
            if (parallelPhaseId >= maxParallelPhase) break;
            std::cout   << "RESULT benchmark=sequentialProfiler(" << benchmarkIdentifier << ")"
                        << " timeUnit=microseconds"
                        << " type=parallelPhase"
                        << " sequenceId=" << parallelPhaseId
                        << " identifier="<< ParallelPhaseNames[parallelPhaseId]
                        << " sdsType=" << sds
                        << " iteration=" << iteration
                        << " " << extra;
            period.printResultString();
            std::cout << " " << config << std::endl;
            ++parallelPhaseId;
        }
        size_t eventId = 0;
        for (const std::int64_t count : events) {
            if (eventId >= maxEvent) break;
            std::cout   << "RESULT benchmark=sequentialProfiler(" << benchmarkIdentifier << ")"
                        << " timeUnit=microseconds"
                        << " type=event"
                        << " count=" << count
                        << " sequenceId=" << eventId
                        << " identifier="<< EventNames[eventId]
                        << " sdsType=" << sds
                        << " iteration=" << iteration
                        << " " << extra;
            std::cout << " " << config << std::endl;
            ++eventId;
        }
    }

    template<typename T>
    inline void printEventLines(Configuration<T> &config, std::string benchmarkIdentifier, std::string sds, int iteration, std::string extra = "", size_t maxEvent = 10000) const {
        size_t eventId = 0;
        for (const std::int64_t count : events) {
            if (eventId >= maxEvent) break;
            std::cout   << "RESULT benchmark=sequentialProfiler(" << benchmarkIdentifier << ")"
                        << " timeUnit=microseconds"
                        << " type=event"
                        << " count=" << count
                        << " sequenceId=" << eventId
                        << " identifier="<< EventNames[eventId]
                        << " sdsType=" << sds
                        << " iteration=" << iteration
                        << " " << extra;
            std::cout << " " << config << std::endl;
            ++eventId;
        }
    }

    SequentialProfiler& operator+=(const SequentialProfiler &other) {
        AssertMsg(operations.size() == other.operations.size(), "Cannot aggregate profilers.");
        AssertMsg(phases.size() == other.phases.size(), "Cannot aggregate profilers.");
        for (size_t i = 0; i < operations.size(); ++i) {
            operations[i] += other.operations[i];
        }
        for (size_t i = 0; i < phases.size(); ++i) {
            phases[i] += other.phases[i];
        }
        for (size_t i = 0; i < parallelPhases.size(); ++i) {
            parallelPhases[i] = std::max(parallelPhases[i], other.parallelPhases[i]);
        }
        for (size_t i = 0; i < events.size(); ++i) {
            events[i] += other.events[i];
        }
        return *this;
    }

    static inline std::string identifier() {
        return "SequentialProfiler";
    }

private:
    inline void printHeader() const {
        std::cout << std::setw(std::max(std::max(phaseWidth(), parallelPhaseWidth()), std::max(operationWidth(), eventWidth()))) << "Period"
            << std::setw(maxValueWidth) << "TotalTime (µs)"
            << std::setw(maxValueWidth) << "#Calls"
            << std::setw(maxValueWidth) << "AvgTime (µs)";
    }
};

class ConcurrentProfiler {
private:
    std::map<std::thread::id, SequentialProfiler> profilers;
    SequentialProfiler aggregateProfiler;
    std::mutex m;

    bool active = false;
public:
    inline void registerThread() {
        auto l = std::scoped_lock(m);
        profilers[std::this_thread::get_id()];//Create a sequential profiler for this thread's key
    }

    inline void activate() {
        auto l = std::scoped_lock(m);
        AssertMsg(!active, "Already active.");
        active = true;
        for (auto &[key, profiler] : profilers) {
            profiler.activate();
        }
    }

    inline void deactivate() {
        auto l = std::scoped_lock(m);
        AssertMsg(active, "Not active.");
        active = false;
        for (auto &[key, profiler] : profilers) {
            profiler.deactivate();
        }
    }

    inline void startPhase(Phase phase) {
        if (active) profilers.at(std::this_thread::get_id()).startPhase(phase);
    }

    inline void endPhase(Phase phase) {
        if (active) profilers.at(std::this_thread::get_id()).endPhase(phase);
    }

    inline void startParallelPhase(ParallelPhase phase) {
        if (active) profilers.at(std::this_thread::get_id()).startParallelPhase(phase);
    }

    inline void endParallelPhase(ParallelPhase phase) {
        if (active) profilers.at(std::this_thread::get_id()).endParallelPhase(phase);
    }

    inline void startOperation(Operation op) {
        if (active) profilers.at(std::this_thread::get_id()).startOperation(op);
    }

    inline void endOperation(Operation op) {
        if (active) profilers.at(std::this_thread::get_id()).endOperation(op);
    }

    inline void countEvent(Event event) {
        if (active) profilers.at(std::this_thread::get_id()).countEvent(event);
    }

    inline void print() {
        auto l = std::scoped_lock(m);
        aggregateProfiler.reset();
        for (const auto &[key, profiler] : profilers) {
            aggregateProfiler += profiler;
        }
        printHeader();
        for (size_t opId = 0; opId < opCount; ++opId) {
            if (aggregateProfiler.operations[opId].wasUsed()) {
                std::cout << std::setw(std::max(std::max(phaseWidth(), parallelPhaseWidth()), std::max(operationWidth(), eventWidth()))) << OperationNames[opId];
                size_t threadId = 0;
                for (const auto &[key, profiler]: profilers) {
                    profiler.printOperation(opId);
                    if (++threadId == NumberOfThreadsPrinted) break;
                }
                aggregateProfiler.printOperation(opId);
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
        for (size_t phaseId = 0; phaseId < phaseCount; ++phaseId) {
            if (aggregateProfiler.phases[phaseId].wasUsed()) {
                std::cout << std::setw(std::max(std::max(phaseWidth(), parallelPhaseWidth()), std::max(operationWidth(), eventWidth()))) << PhaseNames[phaseId];
                size_t threadId = 0;
                for (const auto &[key, profiler]: profilers) {
                    profiler.printPhase(phaseId);
                    if (++threadId == NumberOfThreadsPrinted) break;
                }
                aggregateProfiler.printPhase(phaseId);
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
        for (size_t parallelPhaseId = 0; parallelPhaseId < parallelPhaseCount; ++parallelPhaseId) {
            if (aggregateProfiler.parallelPhases[parallelPhaseId].wasUsed()) {
                std::cout << std::setw(std::max(std::max(phaseWidth(), parallelPhaseWidth()), std::max(operationWidth(), eventWidth()))) << ParallelPhaseNames[parallelPhaseId];
                size_t threadId = 0;
                for (const auto &[key, profiler]: profilers) {
                    profiler.printParallelPhase(parallelPhaseId);
                    if (++threadId == NumberOfThreadsPrinted) break;
                }
                aggregateProfiler.printParallelPhase(parallelPhaseId);
                std::cout << std::endl;
            }
        }
        std::cout << std::endl;
        for (size_t eventId = 0; eventId < eventCount; ++eventId) {
            if (aggregateProfiler.events[eventId] > 0) {
                std::cout << std::setw(std::max(std::max(phaseWidth(), parallelPhaseWidth()), std::max(operationWidth(), eventWidth()))) << EventNames[eventId];
                size_t threadId = 0;
                for (const auto &[key, profiler]: profilers) {
                    profiler.printEvent(eventId);
                    if (++threadId == NumberOfThreadsPrinted) break;
                }
                aggregateProfiler.printEvent(eventId);
                std::cout << std::endl;
            }
        }
    }

    template<typename T>
    inline void printResultLines(Configuration<T> &config, std::string benchmarkIdentifier, std::string sds, int iteration, std::string extra = "") {
        auto l = std::scoped_lock(m);
        aggregateProfiler.reset();
        for (const auto &[key, profiler] : profilers) {
            aggregateProfiler += profiler;
        }
        aggregateProfiler.printResultLines(config, benchmarkIdentifier, sds, iteration, extra);
    }

    static inline std::string identifier() {
        return "ConcurrentProfiler";
    }

private:
    inline void printHeader() const {
        std::cout << std::setw(std::max(std::max(phaseWidth(), parallelPhaseWidth()), std::max(operationWidth(), eventWidth()))) << "Thread";
        std::cout << std::setfill('-');
        for (size_t threadId = 0; threadId < std::min(profilers.size(), NumberOfThreadsPrinted); ++threadId) {
            std::cout << "  " << std::setw(3 * maxValueWidth - 2) << threadId;
        }
        std::cout << "  " << std::setw(3 * maxValueWidth - 2) << "Sum / Max(parallel)";
        std::cout << std::setfill(' ') << std::endl;
        std::cout << std::setw(std::max(std::max(phaseWidth(), parallelPhaseWidth()), std::max(operationWidth(), eventWidth()))) << "Period";
        for (size_t threadId = 0; threadId < std::min(profilers.size(), NumberOfThreadsPrinted) + 1; ++threadId) {
            std::cout << std::setw(maxValueWidth) << "TotalTime"
                << std::setw(maxValueWidth) << "#Calls"
                << std::setw(maxValueWidth) << "AvgTime";
        }
        std::cout << std::endl;
    }
};
}