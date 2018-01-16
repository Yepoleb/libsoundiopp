cmake_minimum_required (VERSION 3.1)
project (soundiopp)

include_directories (include src)

set (CPP_SOURCES
    src/soundiopp.cpp
    src/channellayout.cpp
    src/context.cpp
    src/device.cpp
    src/instream.cpp
    src/outstream.cpp
    src/ringbuffer.cpp)

set (BUILD_SHARED_LIBS TRUE)

add_library (${PROJECT_NAME} ${CPP_SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED TRUE)
