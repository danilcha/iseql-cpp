add_subdirectory(lib/googletest)



# Now simply link against gtest or gtest_main as needed. Eg
#add_executable(example example.cpp)
#target_link_libraries(example gtest_main)
#add_test(NAME example_test COMMAND example)


enable_testing()
#find_package(GTest REQUIRED)
add_executable(tests model/IntervalTest.cpp)
#target_link_libraries(tests GTest::GTest GTest::Main)
target_link_libraries(tests gtest_main)
add_test(NAME tests COMMAND tests)
