#ifndef __DREAM_COMPILECONFIG
#define __DREAM_COMPILECONFIG
#define DREAM_SDK 0.1.1

#if defined(_UNICODE)||define(UNICODE)
#define USE_UNICODE
#endif

#if (defined(_WIN32)||defined(_WIN64)||defined(WIN32)||defined(WIN64))&&!(defined(_ANDROID))
#define _DREAM_WINDOWS_API
#define _DREAM_WINDOWS
#define _DREAM_COMPILE_WITH_WINDOWS_DEVICE_
#define _DREAM_COMPILE_WITH_DIRECT3D_9_
#endif


#if !defined(_DREAM_WINDOWS_API)
#ifdef _ANDROID
#define _DREAM_ANDROID_PLATFORM_
#define _COMPILE_WITH_OPGLES_DRIVER_
#define _DREAM_COMPILE_WITH_ANDROID_DEVICE_
#else
#define _DREAM_LINUX_PLATFORM_
#define _DREAM_COMPILE_WITH_X11_DEVICE_
#endif
#define _DREAM_POSIX_API_
#endif

#if defined(_MSC_VER)&&(_MSC_VER > 1400)
#define USE_MSVC
#elif defined(__GNUC__)
	#ifdef __i386__
		#define USE_GNUCX86
	#elif defined(__x86_64__)
		#define USE_GNUCX64
	#else
		#error "cannot find this compiler type"
	#endif
#else
#error "cannot support this compiler!"
#endif

#define __DREAM_COMPILE_WITH_ZIP_ARCHIVE_LOADER_
#define _DREAM_COMPILE_WITH_ZLIB_
#define _DREAM_DEPRECATED_

#define _DREAM_COMPILE_WITH_BMP_LOADER_
#define _DREAM_COMPILE_WITH_BMP_WRITER_
#define _DREAM_MATERIAL_MAX_TEXTURES_		4
#endif
