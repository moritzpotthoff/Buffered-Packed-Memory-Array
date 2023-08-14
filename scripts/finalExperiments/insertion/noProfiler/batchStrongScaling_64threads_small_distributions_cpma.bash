rm ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 uniform 100000000 1000 1 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 uniform 100000000 1000 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 uniform 100000000 1000 4 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 uniform 100000000 1000 8 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 uniform 100000000 1000 16 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 uniform 100000000 1000 32 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 uniform 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=128
./build/benchmark/CPMABenchmark 5 uniform 100000000 1000 128 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 normal 100000000 1000 1 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 normal 100000000 1000 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 normal 100000000 1000 4 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 normal 100000000 1000 8 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 normal 100000000 1000 16 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 normal 100000000 1000 32 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 normal 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=128
./build/benchmark/CPMABenchmark 5 normal 100000000 1000 128 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 1000 1 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 1000 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 1000 4 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 1000 8 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 1000 16 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 1000 32 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=128
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 1000 128 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 zipf 100000000 1000 1 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 zipf 100000000 1000 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 zipf 100000000 1000 4 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 zipf 100000000 1000 8 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 zipf 100000000 1000 16 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 zipf 100000000 1000 32 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 zipf 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=128
./build/benchmark/CPMABenchmark 5 zipf 100000000 1000 128 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 ascending 100000000 1000 1 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 ascending 100000000 1000 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 ascending 100000000 1000 4 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 ascending 100000000 1000 8 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 ascending 100000000 1000 16 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 ascending 100000000 1000 32 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 ascending 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=128
./build/benchmark/CPMABenchmark 5 ascending 100000000 1000 128 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 descending 100000000 1000 1 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 descending 100000000 1000 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 descending 100000000 1000 4 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 descending 100000000 1000 8 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 descending 100000000 1000 16 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 descending 100000000 1000 32 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 descending 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=128
./build/benchmark/CPMABenchmark 5 descending 100000000 1000 128 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 1000 1 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 1000 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 1000 4 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 1000 8 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 1000 16 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 1000 32 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=128
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 1000 128 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 1000 1 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 1000 2 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 1000 4 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 1000 8 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 1000 16 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 1000 32 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt
export PARLAY_NUM_THREADS=128
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 1000 128 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchStrongScalingSmallDistributions_cpma.txt

