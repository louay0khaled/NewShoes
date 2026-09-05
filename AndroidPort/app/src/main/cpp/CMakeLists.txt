cmake_minimum_required(VERSION 3.22.1)
project(cnc_android LANGUAGES CXX)

add_library(cnc_android SHARED
    native_bridge.cpp
)

target_compile_features(cnc_android PRIVATE cxx_std_17)
target_compile_options(cnc_android PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(cnc_android
    android
    EGL
    GLESv2
    log
)
