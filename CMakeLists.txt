cmake_minimum_required(VERSION 4.2)

project(PFM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Automatically find all .cpp files inside the src/ folder
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# 2. Pass the generated list of files into the executable
add_executable(run 
    ${SRC_FILES}
)

target_include_directories(run PRIVATE include)