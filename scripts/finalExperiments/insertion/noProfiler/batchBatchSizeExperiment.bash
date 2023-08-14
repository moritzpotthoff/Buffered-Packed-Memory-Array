export OMP_PLACES={0}:128:1
export OMP_PROC_BIND=true
rm ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

export PARLAY_NUM_THREADS=64
./build/benchmark/BatchBenchmark 5 uniform 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 2000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 uniform 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 2000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/CPMABenchmark 5 uniform 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 2000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 uniform 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt


./build/benchmark/BatchBenchmark 5 ascending 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascending 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascending 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/CPMABenchmark 5 ascending 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascending 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt


./build/benchmark/BatchBenchmark 5 descending 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descending 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descending 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/CPMABenchmark 5 descending 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descending 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt


./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 ascendingStar 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 ascendingStar 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 ascendingStar 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt


./build/benchmark/BatchBenchmark 5 descendingStar 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 descendingStar 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 descendingStar 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/CPMABenchmark 5 descendingStar 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 descendingStar 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt


./build/benchmark/BatchBenchmark 5 zipf 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 zipf 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 zipf 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/CPMABenchmark 5 zipf 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 zipf 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt


./build/benchmark/BatchBenchmark 5 normal 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 normal 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 normal 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/CPMABenchmark 5 normal 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 normal 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt


./build/benchmark/BatchBenchmark 5 denseNormal 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/BatchBenchmark 5 denseNormal 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 denseNormal 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt

./build/benchmark/CPMABenchmark 5 denseNormal 100000000 1000000 64 1.0 0.0 0.0 100 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 200000 64 1.0 0.0 0.0 500 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 100000 64 1.0 0.0 0.0 1000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 20000 64 1.0 0.0 0.0 5000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 10000 64 1.0 0.0 0.0 10000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 20000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 1000 64 1.0 0.0 0.0 100000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 200 64 1.0 0.0 0.0 500000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 20 64 1.0 0.0 0.0 5000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 10 64 1.0 0.0 0.0 10000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 2 64 1.0 0.0 0.0 50000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/CPMABenchmark 5 denseNormal 100000000 1 64 1.0 0.0 0.0 100000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt


