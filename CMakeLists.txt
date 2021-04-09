cmake_minimum_required(VERSION 3.16)
project(PTL)
set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
include_directories(
        ${PROJECT_SOURCE_DIR}/src/
)
set(src_dir
        # ${PROJECT_SOURCE_DIR}/src/main.cpp
        # ${PROJECT_SOURCE_DIR}/src/exceptions.hpp
        # ${PROJECT_SOURCE_DIR}/src/utility.hpp
        # ${PROJECT_SOURCE_DIR}/src/vector.hpp
        # ${PROJECT_SOURCE_DIR}/src/priority_queue.hpp
        # ${PROJECT_SOURCE_DIR}/src/deque.hpp
        ${PROJECT_SOURCE_DIR}/src/map.hpp
)
add_executable(code ${src_dir} src/test.cpp)