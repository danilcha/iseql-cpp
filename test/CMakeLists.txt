cmake_minimum_required(VERSION 2.4)


include_directories(../src)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
endif()


enable_testing()
find_package(GTest REQUIRED)
add_executable(tests model/IntervalTest.cpp)
target_link_libraries(tests GTest::GTest GTest::Main)
add_test(NAME tests COMMAND tests)
