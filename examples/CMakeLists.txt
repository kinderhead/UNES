add_executable(init init.c)
target_link_libraries(init PRIVATE UNES)

add_executable(smb smb.c)
target_link_libraries(smb PRIVATE UNES)
