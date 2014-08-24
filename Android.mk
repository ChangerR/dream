LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#ZLIB_SRC = zlib/compress.c zlib/inftrees.c zlib/trees.c zlib/infback.c zlib/inflate.c \
#		zlib/adler32.c zlib/inffast.c zlib/zutil.c zlib/uncompr.c zlib/crc32.c zlib/deflate.c

BASE_SRC = CZipReader.cpp CFileSystem.cpp CMemoryFile.cpp \
		CColorConverter.cpp CImageLoaderBMP.cpp CLogger.cpp os.cpp CLimitReadFile.cpp CFPSCounter.cpp \
		CVideoModeList.cpp CAttribute.cpp CReadFile.cpp \
		CImageWriterBMP.cpp COSOperator.cpp CWriteFile.cpp CDreamDeviceStub.cpp \
		CFileList.cpp CImage.cpp CNullDriver.cpp dream.cpp
		
GLES2_SRC = COGLES2Driver.cpp COGLES2ExtensionHandler.cpp COGLES2FixedPipelineShader.cpp COGLES2NormalMapRenderer.cpp \
		COGLES2Renderer2D.cpp COGLES2SLMaterialRenderer.cpp	COGLES2Texture.cpp COGLES2ParallaxMapRenderer.cpp
	
ANDROID = 	CDreamDeviceAndroid.cpp android-activity.cpp importgl.c
LOCAL_MODULE := dream

LOCAL_ARM_MODE   := arm 
LOCAL_CFLAGS := -O3 -DANDROID_NDK -D_ANDROID -DDISABLE_IMPORTGL -I./include/ -Wno-error=format-security
LOCAL_CXXFLAGS := -O3 -DANDROID_NDK -D_ANDROID -I./include/ -Wno-error=format-security
LOCAL_SRC_FILES := \
     $(ANDROID) \
     $(BASE_SRC) $(GLES2_SRC)

LOCAL_LDLIBS := -ldl -llog -lGLESv2 -lz
#LOCAL_SHARED_LIBRARIES :=	lua52
include $(BUILD_SHARED_LIBRARY)
#$(call import-module,lua)