add_executable(RadioDj RadioDjMain.cpp
        Services/ShoutService.cpp
        Services/ShoutService.h
        Services/Encoder.cpp
        Services/Encoder.h
        Services/Decoder.cpp Services/Decoder.h Services/Deck.cpp Services/Deck.h Services/SendService.cpp Services/SendService.h)

target_link_libraries(RadioDj
        ${LibShout_LIBRARIES}
        ${LibSox_LIBRARIES}
        -lm
        ${AVUtil_LIBRARIES}
        ${AVCodec_LIBRARIES}
        ${AVFormat_LIBRARIES}
        )

target_include_directories(RadioDj PUBLIC
        ${LibShout_INCLUDE_DIRS}
        ${LibSox_INCLUDE_DIRS}
        ${AVUtil_INCLUDE_DIRS}
        ${AVCodec_INCLUDE_DIRS}
        ${AVFormat_INCLUDE_DIRS}
        )

target_compile_options(RadioDj PUBLIC
        ${LibShout_CFLAGS_OTHER}
        ${LibSox_CFLAGS_OTHER}
        ${AVUtil_CFLAGS_OTHER}
        ${AVCodec_CFLAGS_OTHER}
        ${AVFormat_CFLAGS_OTHER}
        )

