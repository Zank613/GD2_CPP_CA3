cmake_minimum_required(VERSION 4.0)
project(GD2_CPP_CA3)

set(CMAKE_CXX_STANDARD 17)

add_executable(GD2_CPP_CA3 main.cpp
        include/Bug.h
        src/Bug.cpp
        include/Crawler.h
        src/Crawler.cpp)

target_include_directories(GD2_CPP_CA3 PRIVATE include)