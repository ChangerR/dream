#ifndef __DREAM_COMPILECONFIG
#define __DREAM_COMPILECONFIG
#define DREAM_SDK 0.1.1

#if defined(_UNICODE)||define(UNICODE)
#define USE_UNICODE
#endif

#if (defined(_WIN32)||defined(_WIN64)||defined(WIN32)||defined(WIN64))&&!(defined(_ANDROID))
#define _DREAM_WINDOWS_API
#define _DREAM_WINDOWS
#define COMPILE_WITH_WINDOWS_DEVICE
#define COMPILE_WITH_DIRECTX9
#endif

#ifdef _ANDROID
#define COMPILE_WITH_ANDROID
#define COMPILE_WITH_OPGLES
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
#endif
