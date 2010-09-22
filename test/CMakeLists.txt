
#ENABLE_TESTING()

include_directories(../src)
add_executable(test md_frame_header_test.c)
target_link_libraries(test MP3Decoder)
