cmake_minimum_required(VERSION 2.8.4)
project(ResultProcessor CXX)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic -Wall -Wextra -Wconversion")
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-weak-vtables -Wno-missing-noreturn -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-shadow -Wno-old-style-cast -Wno-missing-prototypes -Wno-padded")
endif()


set(SOURCE_FILES

		result-processor.cpp

)


add_executable(result-processor ${SOURCE_FILES})

