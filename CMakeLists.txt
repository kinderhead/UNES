cmake_minimum_required(VERSION 3.0.0)
project(UNES VERSION 0.1.0)

set(CMAKE_C_STANDARD 11)

add_library(UNES src/unes.c)
include_directories(UNES include)

add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
