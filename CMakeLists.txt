cmake_minimum_required(VERSION 3.15)
project(OthelloGame)

# Set the C standard to C99
set(CMAKE_C_STANDARD 99)

# Set the vcpkg toolchain file
set(VCPKG_TARGET_TRIPLET "x64-windows")

# Set the paths for vcpkg
set(CMAKE_TOOLCHAIN_FILE "C:/Users/ASUS/Downloads/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
set(CMAKE_GENERATOR_TOOLSET "host=x64" CACHE STRING "host toolset" FORCE)

# Add the executable
add_executable(test test.c)

# Add include directories for cJSON
target_include_directories(test PRIVATE "C:/Users/ASUS/Downloads/vcpkg/installed/x64-windows/include")

# Add library directories for cJSON
target_link_directories(test PRIVATE "C:/Users/ASUS/Downloads/vcpkg/installed/x64-windows/lib")

# Link against cJSON library
target_link_libraries(test PRIVATE cJSON)


