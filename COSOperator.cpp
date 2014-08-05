// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COSOperator.h"

#ifdef _DREAM_WINDOWS_API
#include <windows.h>
#else
#include <string.h>
#include <unistd.h>
#endif

#if defined(_DREAM_COMPILE_WITH_X11_DEVICE_)
#include "CDreamDeviceLinux.h"
#endif


#if defined(_DREAM_COMPILE_WITH_X11_DEVICE_)
// constructor  linux
	COSOperator::COSOperator(const stringc& osVersion, CDreamDeviceLinux* device)
: OperatingSystem(osVersion), DreamDeviceLinux(device)
{
}
#endif

// constructor
COSOperator::COSOperator(const stringc& osVersion) : OperatingSystem(osVersion)
{
}


//! returns the current operating system version as string.
const stringc& COSOperator::getOperatingSystemVersion() const
{
	return OperatingSystem;
}


//! copies text to the clipboard
void COSOperator::copyToClipboard(const c8* text) const
{
	if (strlen(text)==0)
		return;

#if defined(_DREAM_WINDOWS_API)
	if (!OpenClipboard(NULL) || text == 0)
		return;

	EmptyClipboard();

	HGLOBAL clipbuffer;
	char * buffer;

	clipbuffer = GlobalAlloc(GMEM_DDESHARE, strlen(text)+1);
	buffer = (char*)GlobalLock(clipbuffer);

	strcpy(buffer, text);

	GlobalUnlock(clipbuffer);
	SetClipboardData(CF_TEXT, clipbuffer);
	CloseClipboard();
#elif defined(_DREAM_COMPILE_WITH_X11_DEVICE_)
    if ( DreamDeviceLinux )
        DreamDeviceLinux->copyToClipboard(text);
#else

#endif
}


//! gets text from the clipboard
//! \return Returns 0 if no string is in there.
const c8* COSOperator::getTextFromClipboard() const
{
#if defined(_DREAM_WINDOWS_API)
	if (!OpenClipboard(NULL))
		return 0;

	char * buffer = 0;

	HANDLE hData = GetClipboardData( CF_TEXT );
	buffer = (char*)GlobalLock( hData );
	GlobalUnlock( hData );
	CloseClipboard();
	return buffer;
#elif defined(_DREAM_COMPILE_WITH_X11_DEVICE_)
    if ( DreamDeviceLinux )
        return DreamDeviceLinux->getTextFromClipboard();
    return 0;

#else

	return 0;
#endif
}


bool COSOperator::getProcessorSpeedMHz(u32* MHz) const
{
#if defined(_DREAM_WINDOWS_API)
	LONG Error;

	HKEY Key;
	Error = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
			__TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"),
			0, KEY_READ, &Key);

	if(Error != ERROR_SUCCESS)
		return false;

	DWORD Speed = 0;
	DWORD Size = sizeof(Speed);
	Error = RegQueryValueEx(Key, __TEXT("~MHz"), NULL, NULL, (LPBYTE)&Speed, &Size);

	RegCloseKey(Key);

	if (Error != ERROR_SUCCESS)
		return false;
	else if (MHz)
		*MHz = Speed;
	return true;
#else
	// could probably be read from "/proc/cpuinfo" or "/proc/cpufreq"

	return false;
#endif
}

bool COSOperator::getSystemMemory(u32* Total, u32* Avail) const
{
#if defined(_DREAM_WINDOWS_API)
	MEMORYSTATUS MemoryStatus;
	MemoryStatus.dwLength = sizeof(MEMORYSTATUS);

	// cannot fail
	GlobalMemoryStatus(&MemoryStatus);

	if (Total)
		*Total = (u32)(MemoryStatus.dwTotalPhys>>10);
	if (Avail)
		*Avail = (u32)(MemoryStatus.dwAvailPhys>>10);

	_DREAM_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return true;

#elif defined(_DREAM_POSIX_API)
	// TODO: implement for non-availablity of symbols/features
	return false;
#else
	// TODO: implement for OSX
	return false;
#endif
}
