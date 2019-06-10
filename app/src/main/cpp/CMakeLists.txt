# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set(PATH_TO_CPP ${CMAKE_CURRENT_SOURCE_DIR})
set(PATH_TO_SOIL_PROJECT ${PATH_TO_CPP}/com/mvcoder/thirdparty/soil)

#添加 SOIL 库 用于将图片转成rgb纹理
include_directories(${PATH_TO_SOIL_PROJECT}/)
add_subdirectory(${PATH_TO_SOIL_PROJECT})

include_directories(${PATH_TO_CPP}/)
include_directories(${PATH_TO_CPP}/com/mvcoder/common/)
include_directories(${PATH_TO_CPP}/com/mvcoder/common/egl_core/)

aux_source_directory(${PATH_TO_CPP} SOURCE_FILES)


add_library( # Sets the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${SOURCE_FILES})



# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        native-lib
        soil
        GLESv2
        EGL
        log
        android
        )