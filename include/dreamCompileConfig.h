#ifndef __DREAM_COMPILECONFIG
#define __DREAM_COMPILECONFIG
#define DREAM_SDK_VERSION "0.1.1"
#if defined(_UNICODE)||defined(UNICODE)
#define USE_UNICODE
#endif

#if (defined(_WIN32)||defined(_WIN64)||defined(WIN32)||defined(WIN64))&&!(defined(_ANDROID))
#define _DREAM_WINDOWS_API
#define _DREAM_WINDOWS
#define _DREAM_COMPILE_WITH_WINDOWS_DEVICE_
#define _DREAM_COMPILE_WITH_DIRECT3D_9_
#define _DREAM_COMPILE_WITH_OGLES2_
#define _DREAM_D3D_NO_SHADER_DEBUGGING
#ifndef WIN32_LEAN_AND_MEAN
//#define WIN32_LEAN_AND_MEAN
#endif
#endif


#if !defined(_DREAM_WINDOWS_API)
#ifdef _ANDROID
#define _DREAM_ANDROID_PLATFORM_
#define _DREAM_COMPILE_WITH_OGLES2_
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

#if defined(_DEBUG)
#if defined(_DREAM_WINDOWS_API) && defined(_MSC_VER) && !defined (_WIN32_WCE)
  #if defined(WIN64) || defined(_WIN64) // using portable common solution for x64 configuration
  #include <crtdbg.h>
  #define _DREAM_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {_CrtDbgBreak();}
  #else
  #define _DREAM_DEBUG_BREAK_IF( _CONDITION_ ) if (_CONDITION_) {__asm int 3}
  #endif
#else
#include "assert.h"
#define _DREAM_DEBUG_BREAK_IF( _CONDITION_ ) assert( !(_CONDITION_) );
#endif
#else
#define _DREAM_DEBUG_BREAK_IF( _CONDITION_ )
#endif
