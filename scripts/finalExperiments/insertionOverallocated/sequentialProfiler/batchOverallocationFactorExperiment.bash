export OMP_PLACES={0}:128:1
export OMP_PROC_BIND=true

rm ../master-thesis/eval/data/finalExperiments/insertionOverallocated/sequentialProfiler/batchOverallocationFactorExperiment.txt

./build/benchmark/BatchOverallocationFactorExperiment 1 uniform 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertionOverallocated/sequentialProfiler/batchOverallocationFactorExperiment.txt
./build/benchmark/BatchOverallocationFactorExperiment 1 normal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertionOverallocated/sequentialProfiler/batchOverallocationFactorExperiment.txt
./build/benchmark/BatchOverallocationFactorExperiment 1 denseNormal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertionOverallocated/sequentialProfiler/batchOverallocationFactorExperiment.txt
./build/benchmark/BatchOverallocationFactorExperiment 1 zipf 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertionOverallocated/sequentialProfiler/batchOverallocationFactorExperiment.txt
./build/benchmark/BatchOverallocationFactorExperiment 1 ascending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertionOverallocated/sequentialProfiler/batchOverallocationFactorExperiment.txt
./build/benchmark/BatchOverallocationFactorExperiment 1 descending 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertionOverallocated/sequentialProfiler/batchOverallocationFactorExperiment.txt
./build/benchmark/BatchOverallocationFactorExperiment 1 ascendingStar 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertionOverallocated/sequentialProfiler/batchOverallocationFactorExperiment.txt
./build/benchmark/BatchOverallocationFactorExperiment 1 descendingStar 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/finalExperiments/insertionOverallocated/sequentialProfiler/batchOverallocationFactorExperiment.txt
