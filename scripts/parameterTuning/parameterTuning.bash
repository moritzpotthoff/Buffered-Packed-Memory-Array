export OMP_PLACES={0}:128:1
export OMP_PROC_BIND=true
rm ../master-thesis/eval/data/parameterTuning/testData.txt
./build/benchmark/BatchParameterTuning 5 uniform 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt
./build/benchmark/BatchParameterTuning 5 uniform 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt
./build/benchmark/BatchParameterTuning 5 uniform 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt
./build/benchmark/BatchParameterTuning 5 uniform 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt

./build/benchmark/BatchParameterTuning 5 zipf 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt
./build/benchmark/BatchParameterTuning 5 zipf 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt
./build/benchmark/BatchParameterTuning 5 zipf 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt
./build/benchmark/BatchParameterTuning 5 zipf 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt

./build/benchmark/BatchParameterTuning 5 denseNormal 100000000 100 64 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt
./build/benchmark/BatchParameterTuning 5 denseNormal 100000000 100 16 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt
./build/benchmark/BatchParameterTuning 5 denseNormal 100000000 100 4 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt
./build/benchmark/BatchParameterTuning 5 denseNormal 100000000 100 1 1.0 0.0 0.0 1000000 42 10000000000000 >> ../master-thesis/eval/data/parameterTuning/testData.txt
