cmake_minimum_required(VERSION 3.0.0)
project(Transformation VERSION 0.1.0 LANGUAGES C CXX)

# 启用C++11标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "clang++")

# eigen 路径
include_directories("/opt/homebrew/Cellar/eigen/3.4.0_1/include")

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

add_executable(Transformation main.cpp)
