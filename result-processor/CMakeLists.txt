cmake_minimum_required(VERSION 2.8.4)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors")


add_executable(result-processor result-processor.cpp Table.h TableIO.h FileUtil.h)


#set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED OFF)
#set(Boost_USE_STATIC_RUNTIME    OFF)
find_package(Boost 1.36.0 COMPONENTS filesystem)
if (Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	target_link_libraries(result-processor ${Boost_LIBRARIES})
endif()
