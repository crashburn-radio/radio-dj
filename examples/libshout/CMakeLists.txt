add_executable(libshout_example example.c)
#add_executable(libshout_nonblocking nonblocking.c)

find_package(PkgConfig REQUIRED)

# libshout
target_link_libraries(libshout_example ${LibShout_LIBRARIES})
target_include_directories(libshout_example PUBLIC ${LibShout_INCLUDE_DIRS})
target_compile_options(libshout_example PUBLIC ${LibShout_CFLAGS_OTHER})

