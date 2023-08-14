export OMP_PLACES={0}:128:1
export OMP_PROC_BIND=true

rm ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt

./build/benchmark/BatchBenchmark 5 uniform 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 100 128 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt

./build/benchmark/BatchBenchmark 5 normal 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 100 128 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt

./build/benchmark/BatchBenchmark 5 denseNormal 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 100 128 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt

./build/benchmark/BatchBenchmark 5 zipf 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 100 128 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt

./build/benchmark/BatchBenchmark 5 ascending 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 100 128 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt

./build/benchmark/BatchBenchmark 5 descending 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 100 128 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt

./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 100 128 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt

./build/benchmark/BatchBenchmark 5 descendingStar 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 100 128 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/scanConfig/noProfiler/batchStrongScalingLargeDistributionsScan.txt