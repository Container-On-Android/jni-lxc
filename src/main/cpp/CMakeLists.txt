cmake_minimum_required(VERSION 3.22.1)

project("lxc-wrapper")

set(DEPS_LIB_DIR ${CMAKE_SOURCE_DIR}/deps/arm64-v8a)

# static liblxc
set(LXC_STATIC ${DEPS_LIB_DIR}/liblxc.a)

# copy from /system/lib64 in Android 13
set(LIBIBCPP_SO    ${DEPS_LIB_DIR}/libc++.so)
set(LIBPCRE2_SO    ${DEPS_LIB_DIR}/libpcre2.so)
set(LIBPKGLIST_SO  ${DEPS_LIB_DIR}/libpackagelistparser.so)
set(LIBCAP_SO      ${DEPS_LIB_DIR}/libcap.so)
set(LIBSSL_SO      ${DEPS_LIB_DIR}/libssl.so)
set(LIBCRYPTO_SO   ${DEPS_LIB_DIR}/libcrypto.so)
set(LIBMINIJAIL_SO ${DEPS_LIB_DIR}/libminijail.so)
set(LIBSELINUX_SO  ${DEPS_LIB_DIR}/libselinux.so)

add_library(lxc-static STATIC IMPORTED)
set_target_properties(lxc-static PROPERTIES
    IMPORTED_LOCATION ${LXC_STATIC}
)

add_library(c++ SHARED IMPORTED)
set_target_properties(c++ PROPERTIES
    IMPORTED_LOCATION ${LIBIBCPP_SO}
)

add_library(pcre2 SHARED IMPORTED)
set_target_properties(pcre2 PROPERTIES
    IMPORTED_LOCATION ${LIBPCRE2_SO}
)

add_library(packagelistparser SHARED IMPORTED)
set_target_properties(packagelistparser PROPERTIES
    IMPORTED_LOCATION ${LIBPKGLIST_SO}
)

add_library(cap-shared SHARED IMPORTED)
set_target_properties(cap-shared PROPERTIES
    IMPORTED_LOCATION ${LIBCAP_SO}
)

add_library(ssl-shared SHARED IMPORTED)
set_target_properties(ssl-shared PROPERTIES
    IMPORTED_LOCATION ${LIBSSL_SO}
)

add_library(crypto-shared SHARED IMPORTED)
set_target_properties(crypto-shared PROPERTIES
    IMPORTED_LOCATION ${LIBCRYPTO_SO}
)

add_library(minijail-shared SHARED IMPORTED)
set_target_properties(minijail-shared PROPERTIES
    IMPORTED_LOCATION ${LIBMINIJAIL_SO}
)

add_library(selinux-shared SHARED IMPORTED)
set_target_properties(selinux-shared PROPERTIES
    IMPORTED_LOCATION ${LIBSELINUX_SO}
)

add_library(
    lxc-wrapper
    SHARED
    src/lxc-binding.c
    src/lxc-wrapper.c
)

target_include_directories(lxc-wrapper PRIVATE src/include)

find_library(log log)

# NDK lib -> system lib -> liblxc
target_link_libraries(
    lxc-wrapper
    ${log}
    c++
    pcre2
    packagelistparser
    cap-shared
    ssl-shared
    crypto-shared
    minijail-shared
    selinux-shared
    lxc-static
)
