cmake_minimum_required(VERSION 3.6)
project(root CXX)

set(CMAKE_CXX_STANDARD 14)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")





add_subdirectory(src)
add_subdirectory(test)
