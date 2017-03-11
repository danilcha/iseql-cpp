add_subdirectory(lib/googletest)


include_directories(../src)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors")
endif()


enable_testing()
add_executable(tests model/IntervalTest.cpp algorithms/AlgorithmsTest.cpp)
target_link_libraries(tests gmock_main)
add_test(NAME tests COMMAND tests)
