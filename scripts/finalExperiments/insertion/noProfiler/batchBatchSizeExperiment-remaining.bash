export OMP_PLACES={0}:128:1
export OMP_PROC_BIND=true
export PARLAY_NUM_THREADS=64

./build/benchmark/BatchBenchmark 5 uniform 100000000 2000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
./build/benchmark/ParallelBSTBenchmark 5 uniform 100000000 2000 64 1.0 0.0 0.0 50000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertion/noProfiler/batchBatchSizeExperiment.txt
