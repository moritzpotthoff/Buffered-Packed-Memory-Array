echo "Declarations:"
for density in "DensityOne" "DensityTwo" "DensityThree"
do
    for segmentSize in 256 512 1024 2048 4096
    do 
        for blockSize in 16 32 64 128 256 512 1024 2048 4096
        do 
            echo "using SearchDataStructureDensity"$density"Segment"$segmentSize"Block"$blockSize" = DataStructures::BatchedSparseArray<Benchmark::DataType, Benchmark::DataType, 0, Benchmark::KeyOf, std::less<Benchmark::DataType>, AllocatorType, 128, "$blockSize", "$segmentSize", DataStructures::"$density", Helpers::Profiler::NoProfiler, Helpers::NoDebugStream<std::thread::id>>;"
        done
        echo ""
    done 
done

echo "Instantiations"
for density in "DensityOne" "DensityTwo" "DensityThree"
do
    for segmentSize in 256 512 1024 2048 4096
    do
        for blockSize in 16 32 64 128 256 512 1024 2048 4096
        do 
            echo "SearchDataStructureDensity"$density"Segment"$segmentSize"Block"$blockSize" sdsDensity"$density"Segment"$segmentSize"Block"$blockSize"{alloc, config.numThreads};"
        done
        echo ""
    done 
done

echo "List for function calls"
for density in "DensityOne" "DensityTwo" "DensityThree"
do
    for segmentSize in 256 512 1024 2048 4096
    do
        for blockSize in 16 32 64 128 256 512 1024 2048 4096
        do 
            echo "sdsDensity"$density"Segment"$segmentSize"Block"$blockSize","
        done
        echo ""
    done 
done