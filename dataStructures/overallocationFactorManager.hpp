#pragma once

#include "batchedSparseArray.hpp"
#include "assert.hpp"
#include "typedefs.hpp"

namespace DataStructures {
/**
 * Computes the next overallocation factor to reach a certain frequency of expensive
 * vs. cheap insert operations.
 */
class OverallocationFactorManager {
    static constexpr double MinOverallocationFactor = 1.1;
    static constexpr double MaxOverallocationFactor = 2.8;
    static constexpr double InitialOverallocationFactor = MaxOverallocationFactor;
    static constexpr double MaxRelativeDifferencePerStep = 0.75;
    static constexpr size_t TargetNumberOfInPlaceInserts = 500;

private:
    double overallocationFactor;
    size_t totalInPlaceInserts;
    size_t totalOutOfPlaceInserts;
    size_t currentInPlaceInserts;

public:
    OverallocationFactorManager() :
        overallocationFactor(InitialOverallocationFactor),
        totalInPlaceInserts(0),
        totalOutOfPlaceInserts(0),
        currentInPlaceInserts(TargetNumberOfInPlaceInserts) {
    }

    void registerInPlaceInsert() {
        ++currentInPlaceInserts;
        ++totalInPlaceInserts;
    }

    double getNewOverallocationFactor() {
        //An out-of-place insert starts, calculate new factor.
        double newOverallocationFactor;
        if (currentInPlaceInserts < TargetNumberOfInPlaceInserts) {
            //Not enough in-place inserts, get greater factor
            const size_t diff = TargetNumberOfInPlaceInserts - currentInPlaceInserts;
            const double relativeDifference = std::min(MaxRelativeDifferencePerStep, static_cast<double>(diff) / TargetNumberOfInPlaceInserts);
            newOverallocationFactor = overallocationFactor + relativeDifference * (MaxOverallocationFactor - overallocationFactor);
            //std::cout << "Got " << currentInPlaceInserts << " (wanted " << TargetNumberOfInPlaceInserts << ")." << std::endl;
            //std::cout << "Factor +++: " << overallocationFactor << " --> " << newOverallocationFactor << std::endl;
        } else {
            //Too many in-place inserts, get smaller factor
            const size_t diff = currentInPlaceInserts - TargetNumberOfInPlaceInserts;
            const double relativeDifference = std::min(MaxRelativeDifferencePerStep, static_cast<double>(diff) / TargetNumberOfInPlaceInserts);
            newOverallocationFactor = overallocationFactor - relativeDifference * (overallocationFactor - MinOverallocationFactor);
            //std::cout << "Got " << currentInPlaceInserts << " (wanted " << TargetNumberOfInPlaceInserts << ")." << std::endl;
            //std::cout << "Factor ---: " << overallocationFactor << " --> " << newOverallocationFactor << std::endl;
        }
        currentInPlaceInserts = 0;
        ++totalOutOfPlaceInserts;
        overallocationFactor = newOverallocationFactor;
        return overallocationFactor;
    }

public:
    void printStats() {
        std::cout << "Number of in-place inserts:     " << totalInPlaceInserts << std::endl;
        std::cout << "Number of out-of-place inserts: " << totalOutOfPlaceInserts << std::endl;
    }
};
}
