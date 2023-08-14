#include <boost/histogram.hpp>

#include "thread_coordination.hpp"
#include "definitions.hpp"
#include "operationsGenerator.hpp"
#include "prefillGenerator.hpp"
#include "benchmarks.hpp"
#include "configuration.hpp"
#include "helpers.hpp"
#include "assert.hpp"
#include "timer.hpp"
#include "progress.hpp"

const uint64_t min = 1;
const uint64_t max = 10000000000000;
const uint64_t skewedMax = 100000000000;
const size_t numOps = 8000;
const size_t batchSize = 32768;
const size_t numberOfBuckets = 199;
const uint32_t seed = 42;

using DataType = uint64_t;

int main(int argc, char *argv[]) {
    (void) argc;
    (void) argv;
    for (size_t distribution = 0; distribution < Helpers::numberOfKeyDistributions; ++distribution) {
        std::cout << "Creating histogram for distribution type " << Helpers::KeyDistributionNames[distribution] << std::endl;
        Helpers::Progress progress(numOps * batchSize);
        Helpers::KeyDistribution distr = static_cast<Helpers::KeyDistribution>(distribution);
        uint64_t maxDistr = max;
        if (distr == Helpers::SKEWED_UNIFORM) {
            maxDistr = skewedMax;
        }
        Benchmark::RandomEngine rng;
        rng.seed(seed);
        Helpers::DistributionWrapper <DataType> keyGen(distr, min, maxDistr, numOps, batchSize, rng);
        //Adapted from https://www.boost.org/doc/libs/1_81_0/libs/histogram/doc/html/histogram/getting_started.html (2023-03-15)
        auto h = boost::histogram::make_histogram(boost::histogram::axis::regular<>(numberOfBuckets, min, max, "x"));
        for (size_t i = 0; i < batchSize * numOps; ++i) {
            h(keyGen(rng));//Add values from the key generator to the histogram
            ++progress;
        }
        progress.done();
        for (auto &&x: boost::histogram::indexed(h, boost::histogram::coverage::all)) {
            std::cerr << "RESULT distribution="
                      << Helpers::KeyDistributionNames[distribution]
                      << " min=" << min
                      << " max=" << max
                      << " binMin=" << x.bin().lower()
                      << " binMax=" << x.bin().upper()
                      << " binId=" << x.index()
                      << " value=" << *x
                      << " totalSamples=" << (batchSize * numOps)
                      << " " << std::endl;
        }
    }
}