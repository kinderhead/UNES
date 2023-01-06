cmake_minimum_required(VERSION 3.0.0)
project(UNES VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)

add_library(UNES src/unes.c src/gfx.c)
include_directories(UNES include)

find_package(SDL2 REQUIRED)
include_directories(UNES ${SDL2_INCLUDE_DIRS})
target_link_libraries(UNES ${SDL2_LIBRARIES} m)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/doxygen/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.out)

    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)

    add_custom_target(docs COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif()

add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)