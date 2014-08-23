// Copyright (C) 2002-2008 Nikolaus Gebhardt
// Copyright (C) 2008 Redshift Software, Inc.
// This file is part of the "Dream Engine".
// For conditions of distribution and use, see copyright notice in Dream.h

#include "CDreamDeviceAndroid.h"

#ifdef _DREAM_COMPILE_WITH_ANDROID_DEVICE_

#include "os.h"
	
#include <android/log.h>

IVideoDriver* createOGLES1Driver(const SDreamCreationParameters& params,
	SExposedVideoData& data, IFileSystem* io);
IVideoDriver* createOGLES2Driver(const SDreamCreationParameters& params,
	SExposedVideoData& data, IFileSystem* io);

//! constructor
CDreamDeviceAndroid::CDreamDeviceAndroid(const SDreamCreationParameters& params)
	: CDreamDeviceStub(params)
{
	__android_log_print(ANDROID_LOG_INFO, "Dream", "CDreamDeviceAndroid::CDreamDeviceAndroid");
	
	createDriver();
	
}

//! destructor
CDreamDeviceAndroid::~CDreamDeviceAndroid()
{
}

//! create the driver
void CDreamDeviceAndroid::createDriver()
{
	__android_log_print(ANDROID_LOG_INFO, "Dream", "CDreamDeviceAndroid::createDriver");
	switch(CreationParams.DriverType)
	{
	case EDT_OGLES1:
		#ifdef _DREAM_COMPILE_WITH_OGLES1_
			{
				SExposedVideoData data;
				VideoDriver = createOGLES1Driver(
					CreationParams, data, FileSystem);
			}
		#else
			__android_log_print(ANDROID_LOG_INFO,"Dream","No OpenGL-ES1 support compiled in.");
		#endif
			break;
    case EDT_OGLES2:
        #ifdef _DREAM_COMPILE_WITH_OGLES2_
            {
                SExposedVideoData data;
                VideoDriver = createOGLES2Driver(
                    CreationParams, data, FileSystem);
            }
		#else
            __android_log_print(ANDROID_LOG_INFO,"Dream","No OpenGL-ES2 support compiled in.");
		#endif
			break;
	case EDT_NULL:
		// create null driver
		VideoDriver = createNullDriver(FileSystem, CreationParams.WindowSize);
		break;

	default:
		__android_log_print(ANDROID_LOG_INFO,"Dream","Unable to create video driver of unknown type.");
		break;
	}
}


bool CDreamDeviceAndroid::run()
{
    Timer::tick();
	return true;
}

//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CDreamDeviceAndroid::yield()
{

}

//! Pause execution and let other processes to run for a specified amount of time.
void CDreamDeviceAndroid::sleep(u32 timeMs, bool pauseTimer=false)
{
}

//! sets the caption of the window
void CDreamDeviceAndroid::setWindowCaption(const wchar_t* text)
{
}

//! presents a surface in the client area
bool CDreamDeviceAndroid::present(IImage* surface, void* windowId, rectangle<s32>* srcClip)
{
	return true;
}

//! notifies the device that it should close itself
void CDreamDeviceAndroid::closeDevice()
{
}

//! \return Pointer to a list with all video modes supported
IVideoModeList* CDreamDeviceAndroid::getVideoModeList()
{
	return NULL;
}

//! returns if window is active. if not, nothing need to be drawn
bool CDreamDeviceAndroid::isWindowActive() const
{
	return true;
}


bool CDreamDeviceAndroid::isWindowFocused() const
{
	return isWindowActive();
}


bool CDreamDeviceAndroid::isWindowMinimized() const
{
	return !isWindowActive();
}

//! Set the current Gamma Value for the Display
bool CDreamDeviceAndroid::setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast )
{
	return false;
}

//! Get the current Gamma Value for the Display
bool CDreamDeviceAndroid::getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast )
{
	return false;
}

//! returns color format of the window.
ECOLOR_FORMAT CDreamDeviceAndroid::getColorFormat() const
{
		return CDreamDeviceStub::getColorFormat();
}

//! Sets if the window should be resizeable in windowed mode.
void CDreamDeviceAndroid::setResizable(bool resize)
{
}


//! Minimizes the window
void CDreamDeviceAndroid::minimizeWindow()
{
}


//! Maximizes the window if possible.
void CDreamDeviceAndroid::maximizeWindow()
{
}


//! Restore the window to normal size if possible.
void CDreamDeviceAndroid::restoreWindow()
{
}



#endif // _DREAM_COMPILE_WITH_ANDROID_DEVICE_
