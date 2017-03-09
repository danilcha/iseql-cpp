cmake_minimum_required(VERSION 2.4)


include_directories(../src)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors")
endif()


enable_testing()
find_package(GTest REQUIRED)
find_library(GMOCK_LIBRARY NAMES libgmock.a)
add_executable(tests model/IntervalTest.cpp algorithms/AlgorithmsTest.cpp)
target_link_libraries(tests GTest::GTest GTest::Main ${GMOCK_LIBRARY})
add_test(NAME tests COMMAND tests)
