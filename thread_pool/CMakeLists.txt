cmake_minimum_required (VERSION 2.6)

project (thread_pool C)

include_directories ("include")

add_library (thread_pool "src/platform/linux/thread_pool_linux.c")
target_link_libraries (thread_pool pthread)
