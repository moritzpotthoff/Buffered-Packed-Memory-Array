export OMP_PLACES={0}:128:1
export OMP_PROC_BIND=true

rm ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt

./build/benchmark/BatchBenchmark 5 uniform 200000000 100 1 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 uniform 200000000 100 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 uniform 200000000 100 4 1.0 0.0 0.0 200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 uniform 200000000 100 8 1.0 0.0 0.0 400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 uniform 200000000 100 16 1.0 0.0 0.0 800000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 uniform 200000000 100 32 1.0 0.0 0.0 1600000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 uniform 200000000 100 64 1.0 0.0 0.0 3200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 uniform 200000000 100 128 1.0 0.0 0.0 6400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt

./build/benchmark/BatchBenchmark 5 normal 200000000 100 1 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 normal 200000000 100 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 normal 200000000 100 4 1.0 0.0 0.0 200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 normal 200000000 100 8 1.0 0.0 0.0 400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 normal 200000000 100 16 1.0 0.0 0.0 800000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 normal 200000000 100 32 1.0 0.0 0.0 1600000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 normal 200000000 100 64 1.0 0.0 0.0 3200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 normal 200000000 100 128 1.0 0.0 0.0 6400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt

./build/benchmark/BatchBenchmark 5 denseNormal 200000000 100 1 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 denseNormal 200000000 100 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 denseNormal 200000000 100 4 1.0 0.0 0.0 200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 denseNormal 200000000 100 8 1.0 0.0 0.0 400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 denseNormal 200000000 100 16 1.0 0.0 0.0 800000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 denseNormal 200000000 100 32 1.0 0.0 0.0 1600000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 denseNormal 200000000 100 64 1.0 0.0 0.0 3200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 denseNormal 200000000 100 128 1.0 0.0 0.0 6400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt

./build/benchmark/BatchBenchmark 5 zipf 200000000 100 1 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 zipf 200000000 100 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 zipf 200000000 100 4 1.0 0.0 0.0 200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 zipf 200000000 100 8 1.0 0.0 0.0 400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 zipf 200000000 100 16 1.0 0.0 0.0 800000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 zipf 200000000 100 32 1.0 0.0 0.0 1600000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 zipf 200000000 100 64 1.0 0.0 0.0 3200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 zipf 200000000 100 128 1.0 0.0 0.0 6400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt

./build/benchmark/BatchBenchmark 5 ascending 200000000 100 1 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascending 200000000 100 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascending 200000000 100 4 1.0 0.0 0.0 200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascending 200000000 100 8 1.0 0.0 0.0 400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascending 200000000 100 16 1.0 0.0 0.0 800000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascending 200000000 100 32 1.0 0.0 0.0 1600000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascending 200000000 100 64 1.0 0.0 0.0 3200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascending 200000000 100 128 1.0 0.0 0.0 6400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt

./build/benchmark/BatchBenchmark 5 descending 200000000 100 1 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descending 200000000 100 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descending 200000000 100 4 1.0 0.0 0.0 200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descending 200000000 100 8 1.0 0.0 0.0 400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descending 200000000 100 16 1.0 0.0 0.0 800000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descending 200000000 100 32 1.0 0.0 0.0 1600000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descending 200000000 100 64 1.0 0.0 0.0 3200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descending 200000000 100 128 1.0 0.0 0.0 6400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt

./build/benchmark/BatchBenchmark 5 ascendingStar 200000000 100 1 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 200000000 100 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 200000000 100 4 1.0 0.0 0.0 200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 200000000 100 8 1.0 0.0 0.0 400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 200000000 100 16 1.0 0.0 0.0 800000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 200000000 100 32 1.0 0.0 0.0 1600000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 200000000 100 64 1.0 0.0 0.0 3200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 200000000 100 128 1.0 0.0 0.0 6400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt

./build/benchmark/BatchBenchmark 5 descendingStar 200000000 100 1 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descendingStar 200000000 100 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descendingStar 200000000 100 4 1.0 0.0 0.0 200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descendingStar 200000000 100 8 1.0 0.0 0.0 400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descendingStar 200000000 100 16 1.0 0.0 0.0 800000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descendingStar 200000000 100 32 1.0 0.0 0.0 1600000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descendingStar 200000000 100 64 1.0 0.0 0.0 3200000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt
./build/benchmark/BatchBenchmark 5 descendingStar 200000000 100 128 1.0 0.0 0.0 6400000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchWeakScalingDistributions.txt

