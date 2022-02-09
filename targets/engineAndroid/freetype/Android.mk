LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := freetype
LOCAL_SRC_FILES := $(LOCAL_PATH)/freetype/lib/libfreetype.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/include/freetype

include $(PREBUILT_SHARED_LIBRARY)
