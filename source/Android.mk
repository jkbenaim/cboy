LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := cboy
LOCAL_SRC_FILES := cart.c cpu.c input.c main.c mbc_boot.c mbc.c mbc_cam.c mbc_mbc1.c mbc_mbc2.c mbc_mbc3.c mbc_mbc5.c mbc_none.c memory.c video.c
LOCAL_LDLIBS    := -llog -ljnigraphics

include $(BUILD_SHARED_LIBRARY)