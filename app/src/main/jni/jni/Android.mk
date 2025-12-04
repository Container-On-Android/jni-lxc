LOCAL_PATH := $(call my-dir)

# Precompiled liblxc library module
include $(CLEAR_VARS)
LOCAL_MODULE := lxc-prebuilt
LOCAL_SRC_FILES := ../deps/arm64-v8a/liblxc.a
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../src/include
include $(PREBUILT_STATIC_LIBRARY)

# Main module - lxc-binding (contains two source files)
include $(CLEAR_VARS)
LOCAL_MODULE := lxc-binding

# Add all source files
LOCAL_SRC_FILES := ../src/lxc-binding.c ../src/jni-wrapper.c
LOCAL_CFLAGS    += -Wno-incompatible-pointer-types-discards-qualifiers  # Ignore specific warnings
# Add header file paths
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../src/include

# Link system libraries
LOCAL_LDLIBS := -llog

# Link precompiled liblxc library
LOCAL_STATIC_LIBRARIES := lxc-prebuilt

include $(BUILD_SHARED_LIBRARY)