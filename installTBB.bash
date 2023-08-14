mkdir lib/oneTBBsource/build
cd lib/oneTBBsource/build
cmake -DCMAKE_INSTALL_PREFIX=../../oneTBBinstalled/ -DTBB_TEST=OFF ..
cmake --build . --parallel
cmake --install .