cmake_minimum_required(VERSION 3.17)
project(AllocList)

set(CMAKE_CXX_STANDARD 17)

add_executable(AllocList main.cpp fastallocator.h)