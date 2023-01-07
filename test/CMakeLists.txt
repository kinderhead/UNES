set(TARGET test PROPERTY C_STANDARD 11)

add_executable(test test.c)
target_link_libraries(test PRIVATE UNES)
include_directories(test ../include)
