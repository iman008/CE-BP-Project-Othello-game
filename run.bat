cd build/
if (Test-Path CMakeCache.txt) {
    # If it exists, run cmake --build . --target clean
    cmake --build . --target clean
}
cmake --build  .
Debug\main.exe 