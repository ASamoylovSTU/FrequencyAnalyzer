cmake_minimum_required (VERSION 2.8)
project (Frequency_analyzer)
set (CMAKE_CXX_STANDARD 17)

add_subdirectory (src)

enable_testing ()
add_subdirectory (tests)
