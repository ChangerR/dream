#!/usr/bin bash

ndk-build APP_ABI="armeabi-v7a armeabi" NDK_PROJECT_PATH=${PWD} APP_BUILD_SCRIPT=${PWD}/Android.mk NDK_MODULE_PATH=${PWD} 
if [ $? -eq 0 ]; then
	cp -r libs/ android/
#	rm -rf obj/
	rm -rf libs/
fi

if ! [ -e android/assets/data ]; then
	mkdir -p android/assets/data
	cp COGLES2FixedPipeline.fsh android/assets/data/
	cp	COGLES2FixedPipeline.vsh android/assets/data/
	cp	COGLES2NormalMap.fsh android/assets/data/
	cp	COGLES2NormalMap.vsh android/assets/data/
	cp	COGLES2Renderer2D.fsh android/assets/data/
	cp	COGLES2Renderer2D.vsh android/assets/data/
	cp	source android/assets/data/
fi

cd android&&make ANDROID_HOME=$ANDROID_HOME