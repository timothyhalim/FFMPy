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
include_directories(ffmpeg_x64/include)
link_directories(ffmpeg_x64/lib)

set(SRC
    src/FFMPy.h
    src/FFMPy.cpp
    src/Decoder.h
    src/Decoder.cpp
    src/Decoder_python.h
)

# Tell CMake to create the dll
add_library(${PROJECT} SHARED ${SRC})


# link the module from Qt
target_link_libraries(${PROJECT} ${PYTHON_LIBRARIES})
set_target_properties(
    ${PROJECT} 
    PROPERTIES 
        OUTPUT_NAME ${PROJECT}
        SUFFIX ".pyd"
)