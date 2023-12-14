# Batch-Parallel Packed Memory Array

Code repository for my [master's thesis](https://doi.org/10.5445/IR/1000165546) on batch-parallel packed memory arrays.

## Structure
- ```benchmark/``` is used to run different benchmarks on the different search data structures.
- ```CPMA/``` contains Brian Wheatman's code for the paper *Brian Wheatman, Randal Burns, Aydın Buluç and Helen Xu. "CPMA: An Efficient Batch-Parallel Compressed Set Without Pointers"*, taken from [his repository](https://github.com/wheatman/Packed-Memory-Array).
- ```dataStructures/``` contains my data structures. Code for the BBPMA is contained in ```batchedSparseArray.hpp```, while the GBPMA is contained in ```overallocatedBatchedSparseArray.hpp```.
- ```helpers/``` contains various utility functions.
- ```lib/``` contains libraries.
- ```parallelBST/``` contains a reproduction (with some changes) of Yaroslav Akhremtsev's code for the paper *Yaroslav Akhremtsev and Peter Sanders. "Fast parallel operations on search trees." 2016 IEEE 23rd International Conference on High Performance Computing (HiPC). IEEE, 2016.*, taken from [his repository](https://git.scc.kit.edu/akhremtsev/ParallelBST).
- ```scripts/``` contains a collection of benchmark scripts.
- ```tests/``` contains tests for my data structure and some helpers.

## Usage
For a release build without comparisons to other data structures, use 
```
cmake --preset release
cmake --build --preset release   
```
Comparisons with other data structures are available with the preset ```releaseCompetitors```,
parameter tuning with ```releaseTuning```, and testing with ```releaseTest```.

Benchmarks can be run by executing the respective benchmark scripts.
