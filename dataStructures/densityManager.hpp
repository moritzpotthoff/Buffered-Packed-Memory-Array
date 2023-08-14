#pragma once

#include <cmath>

#include "typedefs.hpp"

namespace DataStructures {
/**
 * A density manager that uses a target density dependent on the length of the interval.
 */
class DensityOne {
private:
    //Density configuration.
    static constexpr double growingFactor = 1.8;
    static constexpr double tauMax = 0.9;
    static constexpr double deltaTau = 1 - tauMax;
public:
    double maxDensity(SizeType level, SizeType height) const {
        return 1 - deltaTau * static_cast<double>(level) / static_cast<double>(height);
    }

    /**
     * Computes the minimum length of a range that contains the
     * given number of blocks.
     */
    SizeType minRangeLength(SizeType numberOfBlocks) const {
        return static_cast<SizeType>(static_cast<double>(numberOfBlocks) * growingFactor);
    }

    static const std::string type() {
        return "densityOne";
    }
};

class DensityTwo {
private:
    //Density configuration.
    static constexpr double growingFactor = 1.2;
    static constexpr double tauMax = 0.9;
    static constexpr double deltaTau = 1 - tauMax;
public:
    double maxDensity(SizeType level, SizeType height) const {
        return 1 - deltaTau * static_cast<double>(level) / static_cast<double>(height);
    }

    /**
     * Computes the minimum length of a range that contains the
     * given number of blocks.
     */
    SizeType minRangeLength(SizeType numberOfBlocks) const {
        return static_cast<SizeType>(static_cast<double>(numberOfBlocks) * growingFactor);
    }

    static const std::string type() {
        return "densityTwo";
    }
};

class DensityThree {
private:
    //Density configuration.
    static constexpr double growingFactor = 1.8;
    static constexpr double tauMax = 0.7;
    static constexpr double deltaTau = 1 - tauMax;
public:
    double maxDensity(SizeType level, SizeType height) const {
        return 1 - deltaTau * static_cast<double>(level) / static_cast<double>(height);
    }

    /**
     * Computes the minimum length of a range that contains the
     * given number of blocks.
     */
    SizeType minRangeLength(SizeType numberOfBlocks) const {
        return static_cast<SizeType>(static_cast<double>(numberOfBlocks) * growingFactor);
    }

    static const std::string type() {
        return "densityThree";
    }
};
}
