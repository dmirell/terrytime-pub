cmake_minimum_required(VERSION 3.20)

# Setup vcpkg script with CMake (note: should be placed before project() call)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE STRING "Vcpkg toolchain file")

project(terrytime)

# Build options
option(BUILD_TESTS "Build tests" ON)

# Set compiling options
set(CMAKE_CXX_STANDARD 17)
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  /W0 /bigobj")
endif()
 
set(Boost_NO_WARN_NEW_VERSIONS true)
# Find packages
find_package(Boost
    COMPONENTS
        serialization
        program_options
        filesystem
    REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(nlohmann_json REQUIRED)

# Build BinAPI lib
add_subdirectory(ext/binapi)

# Include headers
include_directories(include)

# Add sources
if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    # Enable testing for CTest tool
    enable_testing()
endif()
add_subdirectory(src)

# Set the executable
add_executable(terry src/main.cc)

# Link libraries
target_link_libraries(
    terry PRIVATE
    terry_lib
    spdlog::spdlog
)
