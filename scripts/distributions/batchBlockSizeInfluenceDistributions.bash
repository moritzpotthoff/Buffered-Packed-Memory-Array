rm ../master-thesis/eval/data/batchBlockSizeInfluenceDistributions.txt
./build/benchmark/BatchBlockSizeExperiment 10 uniform 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchBlockSizeInfluenceDistributions.txt
./build/benchmark/BatchBlockSizeExperiment 10 skewedUniform 100000000 2000 64 1.0 0.0 0.0 32768 42 100000000000 >> ../master-thesis/eval/data/batchBlockSizeInfluenceDistributions.txt
./build/benchmark/BatchBlockSizeExperiment 10 normal 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchBlockSizeInfluenceDistributions.txt
./build/benchmark/BatchBlockSizeExperiment 10 denseNormal 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchBlockSizeInfluenceDistributions.txt
./build/benchmark/BatchBlockSizeExperiment 10 zipf 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchBlockSizeInfluenceDistributions.txt
./build/benchmark/BatchBlockSizeExperiment 10 ascending 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchBlockSizeInfluenceDistributions.txt
./build/benchmark/BatchBlockSizeExperiment 10 descending 100000000 2000 64 1.0 0.0 0.0 32768 42 10000000000000 >> ../master-thesis/eval/data/batchBlockSizeInfluenceDistributions.txt
