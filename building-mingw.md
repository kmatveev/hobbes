To build Hobbes using MSYS2 UCRT64 environment you will need following packages installed:

mingw-w64-ucrt-x86_64-cmake
mingw-w64-ucrt-x86_64-ninja
mingw-w64-ucrt-x86_64-make
mingw-w64-ucrt-x86_64-llvm
mingw-w64-ucrt-x86_64-zlib
mingw-w64-ucrt-x86_64-zstd
mingw-w64-ucrt-x86_64-termcap
mingw-w64-ucrt-x86_64-readline
mingw-w64-ucrt-x86_64-winpthreads-git

first, generate build script by runing CMake:

cmake .

To specify non-default builder use -G key:

cmake -G "Ninja" .

To generate debug build you should supply a key DCMAKE_BUILD_TYPE

cmake -DCMAKE_BUILD_TYPE=Debug .

Be careful that this key is stored in CMakeCache.txt and will become default for subsequent builds. To revert back to default value you must delete CMakeCache.txt

You can then run build either via CMake or via builder. To run it via CMake use:

cmake --build .

Or running via ninja simply:

ninja

