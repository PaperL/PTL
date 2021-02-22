cmake_minimum_required(VERSION 3.16)
project(PTL)
set(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
include_directories(
        ${PROJECT_SOURCE_DIR}/src/
)
set(src_dir
        ${PROJECT_SOURCE_DIR}/src/main.cpp)
add_executable(code ${src_dir} src/main.cpp)