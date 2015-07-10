cmake_minimum_required (VERSION 2.6)

project (app C)

include_directories ("thread_pool/include")

add_executable (app "main.c")
target_link_libraries (app thread_pool)

add_subdirectory ("thread_pool")
