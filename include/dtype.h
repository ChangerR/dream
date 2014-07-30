#ifndef __D2TYPE__DREAM2
#define __D2TYPE__DREAM2
#include "dreamCompileConfig.h"

typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef char c8;
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
#else
typedef char fschar_t;
#endif

#endif
