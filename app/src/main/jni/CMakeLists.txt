cmake_minimum_required(VERSION 3.28.3)

project("lxc-binding")

set(LXC_LIB_DIR ${CMAKE_SOURCE_DIR}/deps/${ANDROID_ABI})
set(LXC_STATIC_LIB_PATH ${LXC_LIB_DIR}/liblxc.a)

message(STATUS "LXC static library path: ${LXC_STATIC_LIB_PATH}")

add_library(lxc-static STATIC IMPORTED)
set_target_properties(lxc-static PROPERTIES
    IMPORTED_LOCATION ${LXC_STATIC_LIB_PATH}
)

add_library(
    lxc-binding
    SHARED
    src/lxc-binding.c
    src/jni-wrapper.c
)

target_include_directories(lxc-binding PRIVATE src/include)

find_library(
    log-lib
    log
)

target_link_libraries(
    lxc-binding
    ${log-lib}
    lxc-static
)