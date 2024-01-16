cmake_minimum_required(VERSION 3.10.2)

add_library(
    anti_debug
    SHARED
    anti_debug.cpp
)

target_link_libraries(
    anti_debug  # Replace 'your_lib_name' with the name of your native library target
    android        # The 'android' library provides access to Android-specific functionality
    log            # The 'log' library provides access to Android logging functionalities
)
