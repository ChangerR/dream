LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
CORE_SRC	=	lapi.c lcode.c lctype.c ldebug.c ldo.c ldump.c lfunc.c lgc.c llex.c \
	lmem.c lobject.c lopcodes.c lparser.c lstate.c lstring.c ltable.c \
	ltm.c lundump.c lvm.c lzio.c
LIB_SRC	=	lauxlib.c lbaselib.c lbitlib.c lcorolib.c ldblib.c liolib.c \
	lmathlib.c loslib.c lstrlib.c ltablib.c loadlib.c linit.c
LOCAL_MODULE    := lua52
LOCAL_SRC_FILES := \
			$(CORE_SRC) $(LIB_SRC)
LOCAL_C_INCLUDES :=

LOCAL_CFLAGS := -O2 -Wall -DLUA_USE_LINUX -D"getlocaledecpoint()='.'"
 
LOCAL_LDLIBS  := -ldl

include $(BUILD_SHARED_LIBRARY)
