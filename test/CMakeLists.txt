cmake_minimum_required (VERSION 3.8)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

file(GLOB TEST_SRC unit_*.cc)
add_executable(unit "${TEST_SRC}")
target_link_libraries(unit ${GTEST_BOTH_LIBRARIES} pthread libmarchiver)

add_test(unit unit)
