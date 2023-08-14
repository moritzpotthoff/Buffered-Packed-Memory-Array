export PARLAY_NUM_THREADS=64
rm ../master-thesis/eval/data/rangeQuery/profiledSmallBenchmark.txt
./build/benchmark/RangeQueryBenchmark 0 uniform 1000000 1000 64 1.0 0.0 0.0 1000000 42 100000000000 >> ../master-thesis/eval/data/rangeQuery/profiledSmallBenchmark.txt
rm ../master-thesis/eval/data/rangeQuery/profiledMediumBenchmark.txt
./build/benchmark/RangeQueryBenchmark 0 uniform 10000000 1000 64 1.0 0.0 0.0 1000000 42 100000000000 >> ../master-thesis/eval/data/rangeQuery/profiledMediumBenchmark.txt
rm ../master-thesis/eval/data/rangeQuery/profiledLargeBenchmark.txt
./build/benchmark/RangeQueryBenchmark 0 uniform 100000000 1000 64 1.0 0.0 0.0 1000000 42 100000000000 >> ../master-thesis/eval/data/rangeQuery/profiledLargeBenchmark.txt
