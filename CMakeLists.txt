cmake_minimum_required (VERSION 3.8)

project(marchiver)

set (CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED COMPONENTS system)

include_directories(${PROJECT_SOURCE_DIR}/include/)
file(GLOB LIB_SRC ${PROJECT_SOURCE_DIR}/src/*.cc)

add_library(libmarchiver STATIC ${LIB_SRC})

add_executable (marchiver "marchiver.cc")
target_link_libraries(marchiver libmarchiver ${Boost_LIBRARIES})

find_package(Doxygen)

if (DOXYGEN_FOUND)
    message("doxygen found, attempting to build doc")
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    add_custom_target(doc_doxygen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        VERBATIM )
endif (DOXYGEN_FOUND)

enable_testing()
add_subdirectory(test)
