You will need vcpkg installed. In vcpkg you will need following packages:

zstd
zlib
readline
llvm

cmake --toolchain C:\tools\vcpkg\scripts\buildsystems\vcpkg.cmake

cmake --build .  --config Release