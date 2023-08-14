rm ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 uniform 100000000 2000 1 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 uniform 100000000 2000 2 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 uniform 100000000 2000 4 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 uniform 100000000 2000 8 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 uniform 100000000 2000 16 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 uniform 100000000 2000 32 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 uniform 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 normal 100000000 2000 1 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 normal 100000000 2000 2 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 normal 100000000 2000 4 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 normal 100000000 2000 8 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 normal 100000000 2000 16 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 normal 100000000 2000 32 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 normal 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 2000 1 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 2000 2 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 2000 4 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 2000 8 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 2000 16 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 2000 32 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 zipf 100000000 2000 1 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 zipf 100000000 2000 2 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 zipf 100000000 2000 4 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 zipf 100000000 2000 8 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 zipf 100000000 2000 16 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 zipf 100000000 2000 32 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 zipf 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 ascending 100000000 2000 1 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 ascending 100000000 2000 2 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 ascending 100000000 2000 4 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 ascending 100000000 2000 8 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 ascending 100000000 2000 16 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 ascending 100000000 2000 32 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 ascending 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 descending 100000000 2000 1 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 descending 100000000 2000 2 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 descending 100000000 2000 4 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 descending 100000000 2000 8 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 descending 100000000 2000 16 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 descending 100000000 2000 32 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 descending 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 ascending 0 2000 1 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 ascending 0 2000 2 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 ascending 0 2000 4 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 ascending 0 2000 8 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 ascending 0 2000 16 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 ascending 0 2000 32 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 ascending 0 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 descending 0 2000 1 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 descending 0 2000 2 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 descending 0 2000 4 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 descending 0 2000 8 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 descending 0 2000 16 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 descending 0 2000 32 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 descending 0 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt


export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 uniform 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 uniform 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 uniform 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 uniform 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 uniform 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 uniform 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 uniform 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 normal 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 normal 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 normal 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 normal 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 normal 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 normal 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 normal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 zipf 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 zipf 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 zipf 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 zipf 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 zipf 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 zipf 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 zipf 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 ascending 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 ascending 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 ascending 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 ascending 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 ascending 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 ascending 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 ascending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 descending 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 descending 100000000 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 descending 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 descending 100000000 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 descending 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 descending 100000000 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 descending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 ascending 0 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 ascending 0 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 ascending 0 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 ascending 0 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 ascending 0 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 ascending 0 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 ascending 0 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt

export PARLAY_NUM_THREADS=1
./build/benchmark/CPMABenchmark 5 descending 0 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=2
./build/benchmark/CPMABenchmark 5 descending 0 100 2 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=4
./build/benchmark/CPMABenchmark 5 descending 0 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=8
./build/benchmark/CPMABenchmark 5 descending 0 100 8 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=16
./build/benchmark/CPMABenchmark 5 descending 0 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=32
./build/benchmark/CPMABenchmark 5 descending 0 100 32 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
export PARLAY_NUM_THREADS=64
./build/benchmark/CPMABenchmark 5 descending 0 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/batchStrongScalingLargeDistributions_cpma.txt
