export PARLAY_NUM_THREADS=64
rm ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/rangeQuery/largeBenchmark.txt
./build/benchmark/RangeQueryBenchmark 0 uniform 100000000 10000 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/rangeQuery/largeBenchmark.txt

