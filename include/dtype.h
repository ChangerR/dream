#ifndef __DTYPE__DREAM2
#define __DTYPE__DREAM2
#include "dreamCompileConfig.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef char c8;
typedef char s8;
typedef int s32;
typedef unsigned int u32;
typedef float f32;
typedef double f64;

#ifdef USE_GNUCX64
typedef unsigned long u64;
typedef long s64;
#else
typedef unsigned long long u64;
typedef long long s64;
#endif

#ifdef USE_UNICODE
typedef wchar_t fschar_t;
#define _DREAM_TEXT(s) L##s
#define _DREAM_WCHAR_FILESYSTEM
#else
typedef char fschar_t;
#define _DREAM_TEXT(s) s
#endif
#define MAKE_DREAM_ID(c0, c1, c2, c3) \
		((u32)(u8)(c0) | ((u32)(u8)(c1) << 8) | \
		((u32)(u8)(c2) << 16) | ((u32)(u8)(c3) << 24 ))
#endif
