cmake_minimum_required(VERSION 3.0.0)
project(Server VERSION 0.1.0 LANGUAGES C)

set(CMAKE_BUILD_TYPE "DEBUG")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall -O0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -O0")

add_executable(Server main.c)
