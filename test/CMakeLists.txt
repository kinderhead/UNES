add_executable(test test.c)
target_link_libraries(test PRIVATE UNES)
include_directories(test ../include)
