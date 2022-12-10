cmake_minimum_required(VERSION 3.10.0)

set(PROJECT PyExt)

project(${PROJECT})

set(CMAKE_CXX_STANDARD 11)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the Python library
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_DIRS})


set(SRC
    src/PyExt.cpp
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