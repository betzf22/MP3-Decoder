
cmake_minimum_required (VERSION 2.6)

project (mp3-decoder)

SET(CMAKE_C_FLAGS "-std=c99 -g")

add_subdirectory(src)
add_subdirectory(test)
