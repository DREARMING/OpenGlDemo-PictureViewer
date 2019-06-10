cmake_minimum_required(VERSION 3.4.1)

set(PATH_TO_SOIL ${CMAKE_CURRENT_SOURCE_DIR}/)

include_directories(${PATH_TO_SOIL}/)

file(GLOB_RECURSE SOURCE_FILES ${PATH_TO_SOIL}/*.c)

add_library(soil STATIC ${SOURCE_FILES})

target_link_libraries(
        soil
        log
        GLESv2
        EGL
        android
)