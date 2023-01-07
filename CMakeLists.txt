cmake_minimum_required(VERSION 3.24)
project(itmo_comp_arch22_lab4_dayzGoBy)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fopenmp")
set(CMAKE_CPP_COMPILER gcc)

add_executable(itmo_comp_arch22_lab4_dayzGoBy
        hard.cpp)
