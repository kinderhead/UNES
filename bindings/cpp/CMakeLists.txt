add_library(cpp INTERFACE)
set_property(TARGET cpp PROPERTY CXX_STANDARD 17)
target_include_directories(cpp INTERFACE include ../../include)
target_link_libraries(cpp INTERFACE UNES)

add_subdirectory(test)
