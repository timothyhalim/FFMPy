cmake_minimum_required(VERSION 3.10.0)

set(PROJECT FFMPy)

project(${PROJECT})

set(CMAKE_CXX_STANDARD 11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the Python library
find_package(PythonLibs REQUIRED)
include_directories(
    ${PYTHON_INCLUDE_DIRS}
)

# Find FFMPEG library
set(FFMPEG ffmpeg_x64)
include_directories(${FFMPEG}/include)

find_path(AVCODEC_INCLUDE_DIR ${FFMPEG}/include/libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY ${FFMPEG}/lib/avcodec.lib)

find_path(AVDEVICE_INCLUDE_DIR ${FFMPEG}/include/libavdevice/avdevice.h)
find_library(AVDEVICE_LIBRARY ${FFMPEG}/lib/avdevice.lib)

find_path(AVFILTER_INCLUDE_DIR ${FFMPEG}/include/libavfilter/avfilter.h)
find_library(AVFILTER_LIBRARY ${FFMPEG}/lib/avfilter.lib)

find_path(AVFORMAT_INCLUDE_DIR ${FFMPEG}/include/libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY ${FFMPEG}/lib/avformat.lib)

find_path(AVUTIL_INCLUDE_DIR ${FFMPEG}/include/libavutil/avutil.h)
find_library(AVUTIL_LIBRARY ${FFMPEG}/lib/avutil.lib)

find_path(SWRESAMPLE_INCLUDE_DIR ${FFMPEG}/include/libswresample/swresample.h)
find_library(SWRESAMPLE_LIBRARY ${FFMPEG}/lib/swresample.lib)

find_path(SWSCALE_INCLUDE_DIR ${FFMPEG}/include/libswscale/swscale.h)
find_library(SWSCALE_LIBRARY ${FFMPEG}/lib/swscale.lib)

set(SRC
    src/FFMPy.h
    src/FFMPy.cpp
    src/Decoder.h
    src/Decoder.cpp
    src/Decoder_python.h
)

# Tell CMake to create the dll
add_library(${PROJECT} SHARED ${SRC})

target_include_directories(
    ${PROJECT} PRIVATE 

    ${PYTHON_INCLUDE_DIRS}
    ${AVCODEC_INCLUDE_DIR}
    ${AVDEVICE_INCLUDE_DIR}
    ${AVFILTER_INCLUDE_DIR}
    ${AVFORMAT_INCLUDE_DIR}
    ${AVUTIL_INCLUDE_DIR}
    ${SWRESAMPLE_INCLUDE_DIR}
    ${SWSCALE_INCLUDE_DIR}
)
target_link_libraries(
    ${PROJECT} 
    ${PYTHON_LIBRARIES} 
    ${AVCODEC_LIBRARY}
    ${AVDEVICE_LIBRARY}
    ${AVFILTER_LIBRARY}
    ${AVFORMAT_LIBRARY}
    ${AVUTIL_LIBRARY}
    ${SWRESAMPLE_LIBRARY}
    ${SWSCALE_LIBRARY}
)

set_target_properties(
    ${PROJECT} 
    PROPERTIES 
        OUTPUT_NAME ${PROJECT}
        SUFFIX ".pyd"
)

add_custom_command(TARGET ${PROJECT} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/${FFMPEG}/bin"
        $<TARGET_FILE_DIR:${PROJECT}>
    )