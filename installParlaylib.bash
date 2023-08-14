mkdir CPMA/CPMA-test-repo/parlaylib/build
cd CPMA/CPMA-test-repo/parlaylib/build
cmake -DCMAKE_INSTALL_PREFIX=../../../../lib/parlaylib-install/ -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target install