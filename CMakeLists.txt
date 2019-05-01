cmake_minimum_required (VERSION 2.8.11)
set (CMAKE_CXX_STANDARD 14)

project (RadioDj)

# dependencies
# ------------
find_package(PkgConfig REQUIRED)

# sound libs
pkg_check_modules(LibShout REQUIRED shout)
pkg_check_modules(LibSox REQUIRED sox)

# ffmpeg
pkg_check_modules(AVDevice REQUIRED libavdevice)
pkg_check_modules(AVCodec REQUIRED libavcodec)
pkg_check_modules(AVFilter REQUIRED libavfilter)
pkg_check_modules(AVFormat REQUIRED libavformat)
pkg_check_modules(AVResample REQUIRED libavresample)
pkg_check_modules(AVUtil REQUIRED libavutil)
pkg_check_modules(SWResample REQUIRED libswresample)
pkg_check_modules(SWScale REQUIRED libswscale)

# json
pkg_check_modules(LibJansson REQUIRED jansson)

# projects
add_subdirectory (RadioDj)

install(TARGETS RadioDj
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  )
