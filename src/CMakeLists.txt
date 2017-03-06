cmake_minimum_required(VERSION 2.4)

project(ideql CXX)

include_directories(.)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -march=native")





add_executable(iseql

		main.cpp
		model/Interval.h
		model/Tuple.h
		model/Relation.h

		algorithms/BasicStartPreceeding.h

)
