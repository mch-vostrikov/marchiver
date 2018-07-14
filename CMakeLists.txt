cmake_minimum_required (VERSION 3.8)

project(marchiver)

set (CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system)

add_executable (marchiver "marchiver.cc")
target_link_libraries(marchiver ${Boost_LIBRARIES})
