LOCAL_PATH := $(call my-dir)

# JNI Wrapper
include $(CLEAR_VARS)

LOCAL_CFLAGS := -g
ifeq ($(TARGET_ARCH_ABI), armeabi-v7a)
    LOCAL_CFLAGS += -mfloat-abi=softfp -mfpu=neon
endif
LOCAL_MODULE := libarupdater_android
LOCAL_SRC_FILES := JNI/c/ARUPDATER_JNI_Uploader.c JNI/c/ARUPDATER_JNI_Downloader.c JNI/c/ARUPDATER_JNI_Manager.c
LOCAL_LDLIBS := -llog -lz
LOCAL_SHARED_LIBRARIES := libARUpdater-prebuilt libARDiscovery-prebuilt libARSAL-prebuilt
include $(BUILD_SHARED_LIBRARY)
