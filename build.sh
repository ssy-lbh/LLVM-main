cd build
cmake -G "Unix Makefiles"\
    -DLLVM_ENABLE_PROJECTS="clang"\
    -DCMAKE_BUILD_TYPE=Release\
    -DLLVM_TARGETS_TO_BUILD="all"\
    -DBUILD_SHARED_LIBS=On\
    -DLLVM_ENABLE_PLUGINS=On\
    ../llvm

make -j 10
