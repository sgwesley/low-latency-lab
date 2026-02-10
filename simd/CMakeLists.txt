cmake_minimum_required(VERSION 3.14)
project(lottery CXX)

set(CMAKE_CXX_STANDARD 17)

option(BUILD_TESTS "Build tests" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(app src/main.cpp src/lottery_input_reader.h src/lottery_processor.h src/utils.h)
target_compile_options(app PRIVATE -march=native -O3)

if(BUILD_TESTS)
  find_package(GTest REQUIRED)

  enable_testing()

  add_executable(run_tests tests/test_lottery_input_reader.cpp tests/test_lottery_processor.cpp)
  target_compile_options(run_tests PRIVATE -march=native -O3)
  target_link_libraries(run_tests GTest::gtest_main)

  include(GoogleTest)
  gtest_discover_tests(run_tests)
endif()
