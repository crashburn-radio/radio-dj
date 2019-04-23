add_executable(ffmpeg_decode_audio decode_audio.c)

target_link_libraries(ffmpeg_decode_audio ${AVUtil_LIBRARIES} ${AVCodec_LIBRARIES})
target_include_directories(ffmpeg_decode_audio PUBLIC ${AVUtil_INCLUDE_DIRS} ${AVCodec_INCLUDE_DIRS})
target_compile_options(ffmpeg_decode_audio PUBLIC ${AVUtil_CFLAGS_OTHER} ${AVCodec_CFLAGS_OTHER})

add_executable(ffmpeg_encode_audio encode_audio.c)

# -lm is needed for sinus
target_link_libraries(ffmpeg_encode_audio -lm ${AVUtil_LIBRARIES} ${AVCodec_LIBRARIES})
target_include_directories(ffmpeg_encode_audio PUBLIC ${AVUtil_INCLUDE_DIRS} ${AVCodec_INCLUDE_DIRS})
target_compile_options(ffmpeg_encode_audio PUBLIC ${AVUtil_CFLAGS_OTHER} ${AVCodec_CFLAGS_OTHER})
