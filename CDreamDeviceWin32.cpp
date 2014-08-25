// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "DREAMlicht Engine".
// For conditions of distribution and use, see copyright notice in DREAMlicht.h

#include "dreamCompileConfig.h"

#ifdef _DREAM_COMPILE_WITH_WINDOWS_DEVICE_

#include "CDreamDeviceWin32.h"
#include "list.h"
#include "os.h"
#include "CTimer.h"
#include "dstring.h"
#include "COSOperator.h"
#include "dimension2d.h"
#include "IGUISpriteBank.h"
#include <winuser.h>
#include <stdio.h>

#ifdef _MSC_VER
#pragma comment(lib, "winmm.lib")
#endif

#ifdef _DREAM_COMPILE_WITH_DIRECT3D_8_
IVideoDriver* createDirectX8Driver(const SDreamCreationParameters& params,
	IFileSystem* io, HWND window);
#endif

#ifdef _DREAM_COMPILE_WITH_DIRECT3D_9_
IVideoDriver* createDirectX9Driver(const SDreamCreationParameters& params,
	IFileSystem* io, HWND window);
#endif

#ifdef _DREAM_COMPILE_WITH_OPENGL_
IVideoDriver* createOpenGLDriver(const SDreamCreationParameters& params,
	IFileSystem* io, CDreamDeviceWin32* device);
#endif

#ifdef _DREAM_COMPILE_WITH_OGLES2_
IVideoDriver* createOGLES2Driver(const SDreamCreationParameters& params,SExposedVideoData& data, IFileSystem* io);
#endif
// Get the codepage from the locale language id
// Based on the table from http://www.science.co.il/Language/Locale-Codes.asp?s=decimal
static unsigned int LocaleIdToCodepage(unsigned int lcid)
{
	switch ( lcid )
	{
		case 1098:  // Telugu
		case 1095:  // Gujarati
		case 1094:  // Punjabi
		case 1103:  // Sanskrit
		case 1111:  // Konkani
		case 1114:  // Syriac
		case 1099:  // Kannada
		case 1102:  // Marathi
		case 1125:  // Divehi
		case 1067:  // Armenian
		case 1081:  // Hindi
		case 1079:  // Georgian
		case 1097:  // Tamil
			return 0;
		case 1054:  // Thai
			return 874;
		case 1041:  // Japanese
			return 932;
		case 2052:  // Chinese (PRC)
		case 4100:  // Chinese (Singapore)
			return 936;
		case 1042:  // Korean
			return 949;
		case 5124:  // Chinese (Macau S.A.R.)
		case 3076:  // Chinese (Hong Kong S.A.R.)
		case 1028:  // Chinese (Taiwan)
			return 950;
		case 1048:  // Romanian
		case 1060:  // Slovenian
		case 1038:  // Hungarian
		case 1051:  // Slovak
		case 1045:  // Polish
		case 1052:  // Albanian
		case 2074:  // Serbian (Latin)
		case 1050:  // Croatian
		case 1029:  // Czech
			return 1250;
		case 1104:  // Mongolian (Cyrillic)
		case 1071:  // FYRO Macedonian
		case 2115:  // Uzbek (Cyrillic)
		case 1058:  // Ukrainian
		case 2092:  // Azeri (Cyrillic)
		case 1092:  // Tatar
		case 1087:  // Kazakh
		case 1059:  // Belarusian
		case 1088:  // Kyrgyz (Cyrillic)
		case 1026:  // Bulgarian
		case 3098:  // Serbian (Cyrillic)
		case 1049:  // Russian
			return 1251;
		case 8201:  // English (Jamaica)
		case 3084:  // French (Canada)
		case 1036:  // French (France)
		case 5132:  // French (Luxembourg)
		case 5129:  // English (New Zealand)
		case 6153:  // English (Ireland)
		case 1043:  // Dutch (Netherlands)
		case 9225:  // English (Caribbean)
		case 4108:  // French (Switzerland)
		case 4105:  // English (Canada)
		case 1110:  // Galician
		case 10249:  // English (Belize)
		case 3079:  // German (Austria)
		case 6156:  // French (Monaco)
		case 12297:  // English (Zimbabwe)
		case 1069:  // Basque
		case 2067:  // Dutch (Belgium)
		case 2060:  // French (Belgium)
		case 1035:  // Finnish
		case 1080:  // Faroese
		case 1031:  // German (Germany)
		case 3081:  // English (Australia)
		case 1033:  // English (United States)
		case 2057:  // English (United Kingdom)
		case 1027:  // Catalan
		case 11273:  // English (Trinidad)
		case 7177:  // English (South Africa)
		case 1030:  // Danish
		case 13321:  // English (Philippines)
		case 15370:  // Spanish (Paraguay)
		case 9226:  // Spanish (Colombia)
		case 5130:  // Spanish (Costa Rica)
		case 7178:  // Spanish (Dominican Republic)
		case 12298:  // Spanish (Ecuador)
		case 17418:  // Spanish (El Salvador)
		case 4106:  // Spanish (Guatemala)
		case 18442:  // Spanish (Honduras)
		case 3082:  // Spanish (International Sort)
		case 13322:  // Spanish (Chile)
		case 19466:  // Spanish (Nicaragua)
		case 2058:  // Spanish (Mexico)
		case 10250:  // Spanish (Peru)
		case 20490:  // Spanish (Puerto Rico)
		case 1034:  // Spanish (Traditional Sort)
		case 14346:  // Spanish (Uruguay)
		case 8202:  // Spanish (Venezuela)
		case 1089:  // Swahili
		case 1053:  // Swedish
		case 2077:  // Swedish (Finland)
		case 5127:  // German (Liechtenstein)
		case 1078:  // Afrikaans
		case 6154:  // Spanish (Panama)
		case 4103:  // German (Luxembourg)
		case 16394:  // Spanish (Bolivia)
		case 2055:  // German (Switzerland)
		case 1039:  // Icelandic
		case 1057:  // Indonesian
		case 1040:  // Italian (Italy)
		case 2064:  // Italian (Switzerland)
		case 2068:  // Norwegian (Nynorsk)
		case 11274:  // Spanish (Argentina)
		case 1046:  // Portuguese (Brazil)
		case 1044:  // Norwegian (Bokmal)
		case 1086:  // Malay (Malaysia)
		case 2110:  // Malay (Brunei Darussalam)
		case 2070:  // Portuguese (Portugal)
			return 1252;
		case 1032:  // Greek
			return 1253;
		case 1091:  // Uzbek (Latin)
		case 1068:  // Azeri (Latin)
		case 1055:  // Turkish
			return 1254;
		case 1037:  // Hebrew
			return 1255;
		case 5121:  // Arabic (Algeria)
		case 15361:  // Arabic (Bahrain)
		case 9217:  // Arabic (Yemen)
		case 3073:  // Arabic (Egypt)
		case 2049:  // Arabic (Iraq)
		case 11265:  // Arabic (Jordan)
		case 13313:  // Arabic (Kuwait)
		case 12289:  // Arabic (Lebanon)
		case 4097:  // Arabic (Libya)
		case 6145:  // Arabic (Morocco)
		case 8193:  // Arabic (Oman)
		case 16385:  // Arabic (Qatar)
		case 1025:  // Arabic (Saudi Arabia)
		case 10241:  // Arabic (Syria)
		case 14337:  // Arabic (U.A.E.)
		case 1065:  // Farsi
		case 1056:  // Urdu
		case 7169:  // Arabic (Tunisia)
			return 1256;
		case 1061:  // Estonian
		case 1062:  // Latvian
		case 1063:  // Lithuanian
			return 1257;
		case 1066:  // Vietnamese
			return 1258;
	}
	return 65001;   // utf-8
}

struct SEnvMapper
{
	HWND hWnd;
	CDreamDeviceWin32* dreamDev;
};
list<SEnvMapper> EnvMap;
HKL KEYBOARD_INPUT_HKL=0;
unsigned int KEYBOARD_INPUT_CODEPAGE = 1252;
	
SEnvMapper* getEnvMapperFromHWnd(HWND hWnd)
{
	list<SEnvMapper>::node* it = EnvMap.begin();
	for (; it!= EnvMap.end(); it = it->next)
		if ((*it).element.hWnd == hWnd)
			return &(it->element);

	return 0;
}


CDreamDeviceWin32* getDeviceFromHWnd(HWND hWnd)
{
	list<SEnvMapper>::node* it = EnvMap.begin();
	for (; it!= EnvMap.end(); it = it->next)
		if ((*it).element.hWnd == hWnd)
			return (*it).element.dreamDev;

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	#ifndef WM_MOUSEWHEEL
	#define WM_MOUSEWHEEL 0x020A
	#endif
	#ifndef WHEEL_DELTA
	#define WHEEL_DELTA 120
	#endif
	CDreamDeviceWin32* dev;
	switch (message)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		return 0;

	case WM_ERASEBKGND:
		return 0;

	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
		{
				return 0;
		}

	case WM_SIZE:
		{
			// resize
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
				dev->OnResized();
		}
		return 0;

	case WM_DESTROY:
		{
			dev = getDeviceFromHWnd(hWnd);
			if (dev)
				dev->closeDevice();
		}
		return 0;

	case WM_SYSCOMMAND:
		// prevent screensaver or monitor powersave mode from starting
		if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
			(wParam & 0xFFF0) == SC_MONITORPOWER ||
			(wParam & 0xFFF0) == SC_KEYMENU
			)
			return 0;

		break;

	case WM_ACTIVATE:
		// we need to take care for screen changes, e.g. Alt-Tab
		dev = getDeviceFromHWnd(hWnd);
		if (dev && dev->isFullscreen())
		{
			if ((wParam&0xFF)==WA_INACTIVE)
			{
				// If losing focus we minimize the app to show other one
				ShowWindow(hWnd,SW_MINIMIZE);
				// and switch back to default resolution
				dev->switchToFullScreen(true);
			}
			else
			{
				// Otherwise we retore the fullscreen DREAMlicht app
				SetForegroundWindow(hWnd);
				ShowWindow(hWnd, SW_RESTORE);
				// and set the fullscreen resolution again
				dev->switchToFullScreen();
			}
		}
		break;

	case WM_USER:
		return 0;

	case WM_SETCURSOR:
		// because Windows forgot about that in the meantime
		dev = getDeviceFromHWnd(hWnd);
		if (dev)
		{
			dev->getCursorControl()->setActiveIcon( dev->getCursorControl()->getActiveIcon() );
			dev->getCursorControl()->setVisible( dev->getCursorControl()->isVisible() );
		}
		break;

	case WM_INPUTLANGCHANGE:
		// get the new codepage used for keyboard input
		KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
		KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage( LOWORD(KEYBOARD_INPUT_HKL) );
		return 0;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

//! constructor
CDreamDeviceWin32::CDreamDeviceWin32(const SDreamCreationParameters& params)
: CDreamDeviceStub(params), HWnd(0), ChangedToFullScreen(false), Resized(false),
	ExternalWindow(false), Win32CursorControl(0)
{
	// get windows version and create OS operator
	stringc winversion;
	getWindowsVersion(winversion);
	Operator = new COSOperator(winversion);
	Printer::log(winversion.c_str(), ELL_INFORMATION);

	// get handle to exe file
	HINSTANCE hInstance = GetModuleHandle(0);

	// Store original desktop mode.

	memset(&DesktopMode, 0, sizeof(DesktopMode));
	DesktopMode.dmSize = sizeof(DesktopMode);

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &DesktopMode);

	// create the window if we need to and we do not use the null device
	if (!CreationParams.WindowId && CreationParams.DriverType != EDT_NULL)
	{
		const fschar_t* ClassName = __TEXT("CDreamDeviceWin32");

		// Register Class
		WNDCLASSEX wcex;
		wcex.cbSize			= sizeof(WNDCLASSEX);
		wcex.style			= CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc	= WndProc;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 0;
		wcex.hInstance		= hInstance;
		wcex.hIcon			= NULL;
		wcex.hCursor		= 0; // LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
		wcex.lpszMenuName	= 0;
		wcex.lpszClassName	= ClassName;
		wcex.hIconSm		= 0;

		RegisterClassEx(&wcex);

		// calculate client size

		RECT clientSize;
		clientSize.top = 0;
		clientSize.left = 0;
		clientSize.right = CreationParams.WindowSize.Width;
		clientSize.bottom = CreationParams.WindowSize.Height;

		DWORD style = WS_POPUP;

		if (!CreationParams.Fullscreen)
			style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

		AdjustWindowRect(&clientSize, style, FALSE);

		const s32 realWidth = clientSize.right - clientSize.left;
		const s32 realHeight = clientSize.bottom - clientSize.top;

		s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
		s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

		if ( windowLeft < 0 )
			windowLeft = 0;
		if ( windowTop < 0 )
			windowTop = 0;	// make sure window menus are in screen on creation

		if (CreationParams.Fullscreen)
		{
			windowLeft = 0;
			windowTop = 0;
		}

		// create window

		HWnd = CreateWindow( ClassName, __TEXT(""), style, windowLeft, windowTop,
					realWidth, realHeight, NULL, NULL, hInstance, NULL);
		CreationParams.WindowId = HWnd;
//		CreationParams.WindowSize.Width = realWidth;
//		CreationParams.WindowSize.Height = realHeight;

		ShowWindow(HWnd, SW_SHOWNORMAL);
		UpdateWindow(HWnd);

		// fix ugly ATI driver bugs. Thanks to ariaci
		MoveWindow(HWnd, windowLeft, windowTop, realWidth, realHeight, TRUE);

		// make sure everything gets updated to the real sizes
		Resized = true;
	}
	else if (CreationParams.WindowId)
	{
		// attach external window
		HWnd = static_cast<HWND>(CreationParams.WindowId);
		RECT r;
		GetWindowRect(HWnd, &r);
		CreationParams.WindowSize.Width = r.right - r.left;
		CreationParams.WindowSize.Height = r.bottom - r.top;
		CreationParams.Fullscreen = false;
		ExternalWindow = true;
	}

	// create cursor control

	Win32CursorControl = new CCursorControl(this, CreationParams.WindowSize, HWnd, CreationParams.Fullscreen);
	CursorControl = Win32CursorControl;
	// create driver

	createDriver();
	// register environment

	SEnvMapper em;
	em.dreamDev = this;
	em.hWnd = HWnd;
	EnvMap.push_back(em);

	// set this as active window
	if (!ExternalWindow)
	{
		SetActiveWindow(HWnd);
		SetForegroundWindow(HWnd);
	}

	// get the codepage used for keyboard input
	KEYBOARD_INPUT_HKL = GetKeyboardLayout(0);
	KEYBOARD_INPUT_CODEPAGE = LocaleIdToCodepage( LOWORD(KEYBOARD_INPUT_HKL) );

	// inform driver about the window size etc.
	resizeIfNecessary();
}


//! destructor
CDreamDeviceWin32::~CDreamDeviceWin32()
{
	// unregister environment
	list<SEnvMapper>::node* it = EnvMap.begin();
	for (; it!= EnvMap.end(); it = it->next)
	{
		if ((*it).element.hWnd == HWnd)
		{
			EnvMap.erase(it);
			break;
		}
	}

	switchToFullScreen(true);
}


//! create the driver
void CDreamDeviceWin32::createDriver()
{
	switch(CreationParams.DriverType)
	{
	case EDT_DIRECT3D8:
		#ifdef _DREAM_COMPILE_WITH_DIRECT3D_8_

		VideoDriver = createDirectX8Driver(CreationParams, FileSystem, HWnd);

		if (!VideoDriver)
		{
			Printer::log("Could not create DIRECT3D8 Driver.", ELL_ERROR);
		}
		#else
		Printer::log("DIRECT3D8 Driver was not compiled into this dll. Try another one.", ELL_ERROR);
		#endif // _DREAM_COMPILE_WITH_DIRECT3D_8_

		break;

	case EDT_DIRECT3D9:
		#ifdef _DREAM_COMPILE_WITH_DIRECT3D_9_

		VideoDriver = createDirectX9Driver(CreationParams, FileSystem, HWnd);

		if (!VideoDriver)
		{
			Printer::log("Could not create DIRECT3D9 Driver.", ELL_ERROR);
		}
		#else
		Printer::log("DIRECT3D9 Driver was not compiled into this dll. Try another one.", ELL_ERROR);
		#endif // _DREAM_COMPILE_WITH_DIRECT3D_9_

		break;

	case EDT_OPENGL:

		#ifdef _DREAM_COMPILE_WITH_OPENGL_
		switchToFullScreen();

		VideoDriver = createOpenGLDriver(CreationParams, FileSystem, this);
		if (!VideoDriver)
		{
			Printer::log("Could not create OpenGL driver.", ELL_ERROR);
		}
		#else
		Printer::log("OpenGL driver was not compiled in.", ELL_ERROR);
		#endif
		break;

	case EDT_SOFTWARE:

		#ifdef _DREAM_COMPILE_WITH_SOFTWARE_
		switchToFullScreen();

		VideoDriver = createSoftwareDriver(CreationParams.WindowSize, CreationParams.Fullscreen, FileSystem, this);
		#else
		Printer::log("Software driver was not compiled in.", ELL_ERROR);
		#endif

		break;

	case EDT_BURNINGSVIDEO:
		#ifdef _DREAM_COMPILE_WITH_BURNINGSVIDEO_
		switchToFullScreen();

		VideoDriver = createBurningVideoDriver(CreationParams, FileSystem, this);
		#else
		Printer::log("Burning's Video driver was not compiled in.", ELL_ERROR);
		#endif
		break;
	
	case EDT_OGLES2:
		#if defined(_DREAM_COMPILE_WITH_OGLES2_) &&defined(USE_GLES2)
		{
			SExposedVideoData data;
			data.OpenGLWin32.HWnd=HWnd;

			switchToFullScreen();

			VideoDriver = createOGLES2Driver(CreationParams, data, FileSystem);
			if (!VideoDriver)
			{
				Printer::log("Could not create OpenGL-ES1 driver.", ELL_ERROR);
			}
		}
		#else
			Printer::log("OpenGL-ES2 driver was not compiled in.", ELL_ERROR);
		#endif
		break;
	case EDT_NULL:
		// create null driver
		VideoDriver = createNullDriver(FileSystem, CreationParams.WindowSize);
		break;

	default:
		Printer::log("Unable to create video driver of unknown type.", ELL_ERROR);
		break;
	}
}


//! runs the device. Returns false if device wants to be deleted
bool CDreamDeviceWin32::run()
{
	Timer::tick();

	static_cast<CCursorControl*>(CursorControl)->update();

	handleSystemMessages();

	if (!Close)
		resizeIfNecessary();

	return !Close;
}


//! Pause the current process for the minimum time allowed only to allow other processes to execute
void CDreamDeviceWin32::yield()
{
	Sleep(1);
}

//! Pause execution and let other processes to run for a specified amount of time.
void CDreamDeviceWin32::sleep(u32 timeMs, bool pauseTimer)
{
	const bool wasStopped = Timer ? Timer->isStopped() : true;
	if (pauseTimer && !wasStopped)
		Timer->stop();

	Sleep(timeMs);

	if (pauseTimer && !wasStopped)
		Timer->start();
}


void CDreamDeviceWin32::resizeIfNecessary()
{
	if (!Resized || !getVideoDriver())
		return;

	RECT r;
	GetClientRect(HWnd, &r);

	char tmp[255];

	if (r.right < 2 || r.bottom < 2)
	{
		sprintf(tmp, "Ignoring resize operation to (%ld %ld)", r.right, r.bottom);
		Printer::log(tmp);
	}
	else
	{
		sprintf(tmp, "Resizing window (%ld %ld)", r.right, r.bottom);
		Printer::log(tmp);

		getVideoDriver()->OnResize(dimension2du((u32)r.right, (u32)r.bottom));
		getWin32CursorControl()->OnResize(getVideoDriver()->getScreenSize());
	}

	Resized = false;
}


//! sets the caption of the window
void CDreamDeviceWin32::setWindowCaption(const wchar_t* text)
{
	// We use SendMessage instead of SetText to ensure proper
	// function even in cases where the HWND was created in a different thread
	DWORD_PTR dwResult;
	SendMessageTimeoutW(HWnd, WM_SETTEXT, 0,
			reinterpret_cast<LPARAM>(text),
			SMTO_ABORTIFHUNG, 2000, &dwResult);
}


//! presents a surface in the client area
bool CDreamDeviceWin32::present(IImage* image, void* windowId, rectangle<s32>* src)
{
	HWND hwnd = HWnd;
	if ( windowId )
		hwnd = reinterpret_cast<HWND>(windowId);

	HDC dc = GetDC(hwnd);

	if ( dc )
	{
		RECT rect;
		GetClientRect(hwnd, &rect);
		const void* memory = (const void *)image->lock();

		BITMAPV4HEADER bi;
		ZeroMemory (&bi, sizeof(bi));
		bi.bV4Size = sizeof(BITMAPINFOHEADER);
		bi.bV4BitCount = (WORD)image->getBitsPerPixel();
		bi.bV4Planes = 1;
		bi.bV4Width = image->getDimension().Width;
		bi.bV4Height = -((s32)image->getDimension().Height);
		bi.bV4V4Compression = BI_BITFIELDS;
		bi.bV4AlphaMask = image->getAlphaMask();
		bi.bV4RedMask = image->getRedMask();
		bi.bV4GreenMask = image->getGreenMask();
		bi.bV4BlueMask = image->getBlueMask();

		if ( src )
		{
			StretchDIBits(dc, 0,0, rect.right, rect.bottom,
					src->UpperLeftCorner.X, src->UpperLeftCorner.Y,
					src->getWidth(), src->getHeight(),
					memory, (const BITMAPINFO*)(&bi), DIB_RGB_COLORS, SRCCOPY);
		}
		else
		{
			StretchDIBits(dc, 0,0, rect.right, rect.bottom,
					0, 0, image->getDimension().Width, image->getDimension().Height,
					memory, (const BITMAPINFO*)(&bi), DIB_RGB_COLORS, SRCCOPY);
		}

		image->unlock();

		ReleaseDC(hwnd, dc);
	}
	return true;
}


//! notifies the device that it should close itself
void CDreamDeviceWin32::closeDevice()
{
	MSG msg;
	PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	PostQuitMessage(0);
	PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	if (!ExternalWindow)
	{
		DestroyWindow(HWnd);
		const fschar_t* ClassName = __TEXT("CDreamDeviceWin32");
		HINSTANCE hInstance = GetModuleHandle(0);
		UnregisterClass(ClassName, hInstance);
	}
	Close=true;
}


//! returns if window is active. if not, nothing needs to be drawn
bool CDreamDeviceWin32::isWindowActive() const
{
	return (GetActiveWindow() == HWnd);
}


//! returns if window has focus
bool CDreamDeviceWin32::isWindowFocused() const
{
	bool ret = (GetFocus() == HWnd);
	return ret;
}


//! returns if window is minimized
bool CDreamDeviceWin32::isWindowMinimized() const
{
	WINDOWPLACEMENT plc;
	plc.length=sizeof(WINDOWPLACEMENT);
	bool ret=false;
	if (GetWindowPlacement(HWnd,&plc))
		ret=(plc.showCmd & SW_SHOWMINIMIZED)!=0;
	return ret;
}


//! switches to fullscreen
bool CDreamDeviceWin32::switchToFullScreen(bool reset)
{
	if (!CreationParams.Fullscreen)
		return true;

	if (reset)
	{
		if (ChangedToFullScreen)
		{
			return (ChangeDisplaySettings(&DesktopMode,0)==DISP_CHANGE_SUCCESSFUL);
		}
		else
			return true;
	}

	// use default values from current setting

	DEVMODE dm;
	memset(&dm, 0, sizeof(dm));
	dm.dmSize = sizeof(dm);

	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dm);
	dm.dmPelsWidth = CreationParams.WindowSize.Width;
	dm.dmPelsHeight = CreationParams.WindowSize.Height;
	dm.dmBitsPerPel = CreationParams.Bits;
	dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;

	LONG res = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	if (res != DISP_CHANGE_SUCCESSFUL)
	{ // try again without forcing display frequency
		dm.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
		res = ChangeDisplaySettings(&dm, CDS_FULLSCREEN);
	}

	bool ret = false;
	switch(res)
	{
	case DISP_CHANGE_SUCCESSFUL:
		ChangedToFullScreen = true;
		ret = true;
		break;
	case DISP_CHANGE_RESTART:
		Printer::log("Switch to fullscreen: The computer must be restarted in order for the graphics mode to work.", ELL_ERROR);
		break;
	case DISP_CHANGE_BADFLAGS:
		Printer::log("Switch to fullscreen: An invalid set of flags was passed in.", ELL_ERROR);
		break;
	case DISP_CHANGE_BADPARAM:
		Printer::log("Switch to fullscreen: An invalid parameter was passed in. This can include an invalid flag or combination of flags.", ELL_ERROR);
		break;
	case DISP_CHANGE_FAILED:
		Printer::log("Switch to fullscreen: The display driver failed the specified graphics mode.", ELL_ERROR);
		break;
	case DISP_CHANGE_BADMODE:
		Printer::log("Switch to fullscreen: The graphics mode is not supported.", ELL_ERROR);
		break;
	default:
		Printer::log("An unknown error occured while changing to fullscreen.", ELL_ERROR);
		break;
	}
	return ret;
}


//! returns the win32 cursor control
CDreamDeviceWin32::CCursorControl* CDreamDeviceWin32::getWin32CursorControl()
{
	return Win32CursorControl;
}


//! \return Returns a pointer to a list with all video modes supported
//! by the gfx adapter.
IVideoModeList* CDreamDeviceWin32::getVideoModeList()
{
	if (!VideoModeList->getVideoModeCount())
	{
		// enumerate video modes.
		DWORD i=0;
		DEVMODE mode;
		memset(&mode, 0, sizeof(mode));
		mode.dmSize = sizeof(mode);

		while (EnumDisplaySettings(NULL, i, &mode))
		{
			VideoModeList->addMode(dimension2d<u32>(mode.dmPelsWidth, mode.dmPelsHeight),
				mode.dmBitsPerPel);

			++i;
		}

		if (EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &mode))
			VideoModeList->setDesktop(mode.dmBitsPerPel, dimension2d<u32>(mode.dmPelsWidth, mode.dmPelsHeight));
	}

	return VideoModeList;
}

typedef BOOL (WINAPI *PGPI)(DWORD, DWORD, DWORD, DWORD, PDWORD);
// Needed for old windows apis
// depending on the SDK version and compilers some defines might be available
// or not
#ifndef PRODUCT_ULTIMATE
#define PRODUCT_ULTIMATE	0x00000001
#define PRODUCT_HOME_BASIC	0x00000002
#define PRODUCT_HOME_PREMIUM	0x00000003
#define PRODUCT_ENTERPRISE	0x00000004
#define PRODUCT_HOME_BASIC_N	0x00000005
#define PRODUCT_BUSINESS	0x00000006
#define PRODUCT_STARTER		0x0000000B
#endif
#ifndef PRODUCT_ULTIMATE_N
#define PRODUCT_BUSINESS_N	0x00000010
#define PRODUCT_HOME_PREMIUM_N	0x0000001A
#define PRODUCT_ENTERPRISE_N	0x0000001B
#define PRODUCT_ULTIMATE_N	0x0000001C
#endif
#ifndef PRODUCT_STARTER_N
#define PRODUCT_STARTER_N	0x0000002F
#endif
#ifndef PRODUCT_PROFESSIONAL
#define PRODUCT_PROFESSIONAL	0x00000030
#define PRODUCT_PROFESSIONAL_N	0x00000031
#endif
#ifndef PRODUCT_ULTIMATE_E
#define PRODUCT_STARTER_E	0x00000042
#define PRODUCT_HOME_BASIC_E	0x00000043
#define PRODUCT_HOME_PREMIUM_E	0x00000044
#define PRODUCT_PROFESSIONAL_E	0x00000045
#define PRODUCT_ENTERPRISE_E	0x00000046
#define PRODUCT_ULTIMATE_E	0x00000047
#endif

void CDreamDeviceWin32::getWindowsVersion(stringc& out)
{
	OSVERSIONINFOEX osvi;
	PGPI pGPI;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	bOsVersionInfoEx = GetVersionEx((OSVERSIONINFO*) &osvi);
	if (!bOsVersionInfoEx)
	{
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		if (! GetVersionEx((OSVERSIONINFO *) &osvi))
			return;
	}

	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		if (osvi.dwMajorVersion <= 4)
			out.append("Microsoft Windows NT ");
		else
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
			out.append("Microsoft Windows 2000 ");
		else
		if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1)
			out.append("Microsoft Windows XP ");
		else
		if (osvi.dwMajorVersion == 6 )
		{
			if (osvi.dwMinorVersion == 0)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					out.append("Microsoft Windows Vista ");
				else
					out.append("Microsoft Windows Server 2008 ");
			}
			else if (osvi.dwMinorVersion == 1)
			{
				if (osvi.wProductType == VER_NT_WORKSTATION)
					out.append("Microsoft Windows 7 ");
				else
					out.append("Microsoft Windows Server 2008 R2 ");
			}
		}

		if (bOsVersionInfoEx)
		{
			if (osvi.dwMajorVersion == 6)
			{
				DWORD dwType;
				pGPI = (PGPI)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetProductInfo");
				pGPI(osvi.dwMajorVersion, osvi.dwMinorVersion, 0, 0, &dwType);

				switch (dwType)
				{
				case PRODUCT_ULTIMATE:
				case PRODUCT_ULTIMATE_E:
				case PRODUCT_ULTIMATE_N:
					out.append("Ultimate Edition ");
					break;
				case PRODUCT_PROFESSIONAL:
				case PRODUCT_PROFESSIONAL_E:
				case PRODUCT_PROFESSIONAL_N:
					out.append("Professional Edition ");
					break;
				case PRODUCT_HOME_BASIC:
				case PRODUCT_HOME_BASIC_E:
				case PRODUCT_HOME_BASIC_N:
					out.append("Home Basic Edition ");
					break;
				case PRODUCT_HOME_PREMIUM:
				case PRODUCT_HOME_PREMIUM_E:
				case PRODUCT_HOME_PREMIUM_N:
					out.append("Home Premium Edition ");
					break;
				case PRODUCT_ENTERPRISE:
				case PRODUCT_ENTERPRISE_E:
				case PRODUCT_ENTERPRISE_N:
					out.append("Enterprise Edition ");
					break;
				case PRODUCT_BUSINESS:
				case PRODUCT_BUSINESS_N:
					out.append("Business Edition ");
					break;
				case PRODUCT_STARTER:
				case PRODUCT_STARTER_E:
				case PRODUCT_STARTER_N:
					out.append("Starter Edition ");
					break;
				}
			}
#ifdef VER_SUITE_ENTERPRISE
			else
			if (osvi.wProductType == VER_NT_WORKSTATION)
			{
#ifndef __BORLANDC__
				if( osvi.wSuiteMask & VER_SUITE_PERSONAL )
					out.append("Personal ");
				else
					out.append("Professional ");
#endif
			}
			else if (osvi.wProductType == VER_NT_SERVER)
			{
				if( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					out.append("DataCenter Server ");
				else if( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					out.append("Advanced Server ");
				else
					out.append("Server ");
			}
#endif
		}
		else
		{
			HKEY hKey;
			char szProductType[80];
			DWORD dwBufLen;

			RegOpenKeyEx( HKEY_LOCAL_MACHINE,
					__TEXT("SYSTEM\\CurrentControlSet\\Control\\ProductOptions"),
					0, KEY_QUERY_VALUE, &hKey );
			RegQueryValueEx( hKey, __TEXT("ProductType"), NULL, NULL,
					(LPBYTE) szProductType, &dwBufLen);
			RegCloseKey( hKey );

			if (_strcmpi( "WINNT", szProductType) == 0 )
				out.append("Professional ");
			if (_strcmpi( "LANMANNT", szProductType) == 0)
				out.append("Server ");
			if (_strcmpi( "SERVERNT", szProductType) == 0)
				out.append("Advanced Server ");
		}

		// Display version, service pack (if any), and build number.

		char tmp[255];

		if (osvi.dwMajorVersion <= 4 )
		{
			sprintf(tmp, "version %ld.%ld %s (Build %ld)",
					osvi.dwMajorVersion,
					osvi.dwMinorVersion,
					stringc(osvi.szCSDVersion).c_str(),
					osvi.dwBuildNumber & 0xFFFF);
		}
		else
		{
			sprintf(tmp, "%s (Build %ld)", stringc(osvi.szCSDVersion).c_str(),
			osvi.dwBuildNumber & 0xFFFF);
		}

		out.append(tmp);
		break;

	case VER_PLATFORM_WIN32_WINDOWS:

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 0)
		{
			out.append("Microsoft Windows 95 ");
			if ( osvi.szCSDVersion[1] == 'C' || osvi.szCSDVersion[1] == 'B' )
				out.append("OSR2 " );
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 10)
		{
			out.append("Microsoft Windows 98 ");
			if ( osvi.szCSDVersion[1] == 'A' )
				out.append( "SE " );
		}

		if (osvi.dwMajorVersion == 4 && osvi.dwMinorVersion == 90)
			out.append("Microsoft Windows Me ");

		break;

	case VER_PLATFORM_WIN32s:
		out.append("Microsoft Win32s ");
		break;
	}
}

//! Notifies the device, that it has been resized
void CDreamDeviceWin32::OnResized()
{
	Resized = true;
}

//! Sets if the window should be resizable in windowed mode.
void CDreamDeviceWin32::setResizable(bool resize)
{
	if (ExternalWindow || !getVideoDriver() || CreationParams.Fullscreen)
		return;

	LONG_PTR style = WS_POPUP;

	if (!resize)
		style = WS_SYSMENU | WS_BORDER | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	else
		style = WS_THICKFRAME | WS_SYSMENU | WS_CAPTION | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

	if (!SetWindowLongPtr(HWnd, GWL_STYLE, style))
		Printer::log("Could not change window style.");

	RECT clientSize;
	clientSize.top = 0;
	clientSize.left = 0;
	clientSize.right = getVideoDriver()->getScreenSize().Width;
	clientSize.bottom = getVideoDriver()->getScreenSize().Height;

	AdjustWindowRect(&clientSize, style, FALSE);

	const s32 realWidth = clientSize.right - clientSize.left;
	const s32 realHeight = clientSize.bottom - clientSize.top;

	const s32 windowLeft = (GetSystemMetrics(SM_CXSCREEN) - realWidth) / 2;
	const s32 windowTop = (GetSystemMetrics(SM_CYSCREEN) - realHeight) / 2;

	SetWindowPos(HWnd, HWND_TOP, windowLeft, windowTop, realWidth, realHeight,
		SWP_FRAMECHANGED | SWP_NOMOVE | SWP_SHOWWINDOW);

	static_cast<CCursorControl*>(CursorControl)->updateBorderSize(CreationParams.Fullscreen, resize);
}


//! Minimizes the window.
void CDreamDeviceWin32::minimizeWindow()
{
	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(HWnd, &wndpl);
	wndpl.showCmd = SW_SHOWMINNOACTIVE;
	SetWindowPlacement(HWnd, &wndpl);
}


//! Maximizes the window.
void CDreamDeviceWin32::maximizeWindow()
{
	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(HWnd, &wndpl);
	wndpl.showCmd = SW_SHOWMAXIMIZED;
	SetWindowPlacement(HWnd, &wndpl);
}


//! Restores the window to its original size.
void CDreamDeviceWin32::restoreWindow()
{
	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(HWnd, &wndpl);
	wndpl.showCmd = SW_SHOWNORMAL;
	SetWindowPlacement(HWnd, &wndpl);
}


//! Set the current Gamma Value for the Display
bool CDreamDeviceWin32::setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast )
{
	bool r;
	u16 ramp[3][256];

	calculateGammaRamp( ramp[0], red, brightness, contrast );
	calculateGammaRamp( ramp[1], green, brightness, contrast );
	calculateGammaRamp( ramp[2], blue, brightness, contrast );

	HDC dc = GetDC(0);
	r = SetDeviceGammaRamp ( dc, ramp ) == TRUE;
	ReleaseDC(HWnd, dc);
	return r;
}

//! Get the current Gamma Value for the Display
bool CDreamDeviceWin32::getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast )
{
	bool r;
	u16 ramp[3][256];

	HDC dc = GetDC(0);
	r = GetDeviceGammaRamp ( dc, ramp ) == TRUE;
	ReleaseDC(HWnd, dc);

	if ( r )
	{
		calculateGammaFromRamp(red, ramp[0]);
		calculateGammaFromRamp(green, ramp[1]);
		calculateGammaFromRamp(blue, ramp[2]);
	}

	brightness = 0.f;
	contrast = 0.f;

	return r;

}


//! Process system events
void CDreamDeviceWin32::handleSystemMessages()
{
	MSG msg;

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		// No message translation because we don't use WM_CHAR and it would conflict with our
		// deadkey handling.

		if (ExternalWindow && msg.hwnd == HWnd)
			WndProc(HWnd, msg.message, msg.wParam, msg.lParam);
		else
			DispatchMessage(&msg);

		if (msg.message == WM_QUIT)
			Close = true;
	}
}


//! Remove all messages pending in the system message loop
void CDreamDeviceWin32::clearSystemMessages()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
	{}
	while (PeekMessage(&msg, NULL, WM_MOUSEFIRST, WM_MOUSELAST, PM_REMOVE))
	{}
}

// shows last error in a messagebox to help internal debugging.
void CDreamDeviceWin32::ReportLastWinApiError()
{
	// (based on code from ovidiucucu from http://www.codeguru.com/forum/showthread.php?t=318721)
	LPCTSTR pszCaption = __TEXT("Windows SDK Error Report");
	DWORD dwError = GetLastError();

	if(NOERROR == dwError)
	{
		MessageBox(NULL, __TEXT("No error"), pszCaption, MB_OK);
	}
	else
	{
		const DWORD dwFormatControl = FORMAT_MESSAGE_ALLOCATE_BUFFER |
										FORMAT_MESSAGE_IGNORE_INSERTS |
										FORMAT_MESSAGE_FROM_SYSTEM;

		LPVOID pTextBuffer = NULL;
		DWORD dwCount = FormatMessage(dwFormatControl,
										NULL,
										dwError,
										0,
										(LPTSTR) &pTextBuffer,
										0,
										NULL);
		if(0 != dwCount)
		{
			MessageBox(NULL, (LPCTSTR)pTextBuffer, pszCaption, MB_OK|MB_ICONERROR);
			LocalFree(pTextBuffer);
		}
		else
		{
			MessageBox(NULL, __TEXT("Unknown error"), pszCaption, MB_OK|MB_ICONERROR);
		}
	}
}

// Convert an irrlicht texture to a Windows cursor
// Based on http://www.codeguru.com/cpp/w-p/win32/cursors/article.php/c4529/
HCURSOR CDreamDeviceWin32::TextureToCursor(HWND hwnd, ITexture * tex, const rectangle<s32>& sourceRect, const position2d<s32> &hotspot)
{
	//
	// create the bitmaps needed for cursors from the texture

	HDC dc = GetDC(hwnd);
	HDC andDc = CreateCompatibleDC(dc);
	HDC xorDc = CreateCompatibleDC(dc);
	HBITMAP andBitmap = CreateCompatibleBitmap(dc, sourceRect.getWidth(), sourceRect.getHeight());
	HBITMAP xorBitmap = CreateCompatibleBitmap(dc, sourceRect.getWidth(), sourceRect.getHeight());

	HBITMAP oldAndBitmap = (HBITMAP)SelectObject(andDc, andBitmap);
	HBITMAP oldXorBitmap = (HBITMAP)SelectObject(xorDc, xorBitmap);


	ECOLOR_FORMAT format = tex->getColorFormat();
	u32 bytesPerPixel = IImage::getBitsPerPixelFromFormat(format) / 8;
	u32 bytesLeftGap = sourceRect.UpperLeftCorner.X * bytesPerPixel;
	u32 bytesRightGap = tex->getPitch() - sourceRect.LowerRightCorner.X * bytesPerPixel;
	const u8* data = (const u8*)tex->lock(ETLM_READ_ONLY, 0);
	data += sourceRect.UpperLeftCorner.Y*tex->getPitch();
	for ( s32 y = 0; y < sourceRect.getHeight(); ++y )
	{
		data += bytesLeftGap;
		for ( s32 x = 0; x < sourceRect.getWidth(); ++x )
		{
			SColor pixelCol;
			pixelCol.setData((const void*)data, format);
			data += bytesPerPixel;

			if ( pixelCol.getAlpha() == 0 )	// transparent
			{
				SetPixel(andDc, x, y, RGB(255,255,255));
				SetPixel(xorDc, x, y, RGB(0,0,0));
			}
			else	// color
			{
				SetPixel(andDc, x, y, RGB(0,0,0));
				SetPixel(xorDc, x, y, RGB(pixelCol.getRed(), pixelCol.getGreen(), pixelCol.getBlue()));
			}
		}
		data += bytesRightGap;
	}
	tex->unlock();

	SelectObject(andDc, oldAndBitmap);
	SelectObject(xorDc, oldXorBitmap);

	DeleteDC(xorDc);
	DeleteDC(andDc);

	ReleaseDC(hwnd, dc);

	// create the cursor

	ICONINFO iconinfo;
	iconinfo.fIcon = false;	// type is cursor not icon
	iconinfo.xHotspot = hotspot.X;
	iconinfo.yHotspot = hotspot.Y;
	iconinfo.hbmMask = andBitmap;
	iconinfo.hbmColor = xorBitmap;

	HCURSOR cursor = CreateIconIndirect(&iconinfo);

	DeleteObject(andBitmap);
	DeleteObject(xorBitmap);

	return cursor;
}


CDreamDeviceWin32::CCursorControl::CCursorControl(CDreamDeviceWin32* device, const dimension2d<u32>& wsize, HWND hwnd, bool fullscreen)
	: Device(device), WindowSize(wsize), InvWindowSize(0.0f, 0.0f),
		HWnd(hwnd), BorderX(0), BorderY(0),
		UseReferenceRect(false), IsVisible(true)
		, ActiveIcon(ECI_NORMAL), ActiveIconStartTime(0)
{
	if (WindowSize.Width!=0)
		InvWindowSize.Width = 1.0f / WindowSize.Width;

	if (WindowSize.Height!=0)
		InvWindowSize.Height = 1.0f / WindowSize.Height;

	updateBorderSize(fullscreen, false);
	initCursors();
}

CDreamDeviceWin32::CCursorControl::~CCursorControl()
{
	for ( u32 i=0; i < Cursors.size(); ++i )
	{
		for ( u32 f=0; f < Cursors[i].Frames.size(); ++f )
		{
			DestroyCursor(Cursors[i].Frames[f].IconHW);
		}
	}
}


void CDreamDeviceWin32::CCursorControl::initCursors()
{
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_ARROW)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_CROSS)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_HAND)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_HELP)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_IBEAM)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_NO)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_WAIT)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_SIZEALL)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_SIZENESW)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_SIZENWSE)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_SIZENS)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_SIZEWE)) );
	Cursors.push_back( CursorW32(LoadCursor(NULL, IDC_UPARROW)) );
}


void CDreamDeviceWin32::CCursorControl::update()
{
	if ( !Cursors[ActiveIcon].Frames.empty() && Cursors[ActiveIcon].FrameTime )
	{
		// update animated cursors. This could also be done by X11 in case someone wants to figure that out (this way was just easier to implement)
		u32 now = Device->getTimer()->getRealTime();
		u32 frame = ((now - ActiveIconStartTime) / Cursors[ActiveIcon].FrameTime) % Cursors[ActiveIcon].Frames.size();
		SetCursor( Cursors[ActiveIcon].Frames[frame].IconHW );
	}
}

//! Sets the active cursor icon
void CDreamDeviceWin32::CCursorControl::setActiveIcon(ECURSOR_ICON iconId)
{
	if ( iconId >= (s32)Cursors.size() )
		return;

	ActiveIcon = iconId;
	ActiveIconStartTime = Device->getTimer()->getRealTime();
	if ( Cursors[ActiveIcon].Frames.size() )
		SetCursor( Cursors[ActiveIcon].Frames[0].IconHW );
}


//! Add a custom sprite as cursor icon.
ECURSOR_ICON CDreamDeviceWin32::CCursorControl::addIcon(const SCursorSprite& icon)
{
	if ( icon.SpriteId >= 0 )
	{
		CursorW32 cW32;
		cW32.FrameTime = icon.SpriteBank->getSprites()[icon.SpriteId].frameTime;

		for ( u32 i=0; i < icon.SpriteBank->getSprites()[icon.SpriteId].Frames.size(); ++i )
		{
			u32 texId = icon.SpriteBank->getSprites()[icon.SpriteId].Frames[i].textureNumber;
			u32 rectId = icon.SpriteBank->getSprites()[icon.SpriteId].Frames[i].rectNumber;
			rectangle<s32> rectIcon = icon.SpriteBank->getPositions()[rectId];

			HCURSOR hc = Device->TextureToCursor(HWnd, icon.SpriteBank->getTexture(texId), rectIcon, icon.HotSpot);
			cW32.Frames.push_back( CursorFrameW32(hc) );
		}

		Cursors.push_back( cW32 );
		return (ECURSOR_ICON)(Cursors.size() - 1);
	}
	return ECI_NORMAL;
}


//! replace the given cursor icon.
void CDreamDeviceWin32::CCursorControl::changeIcon(ECURSOR_ICON iconId, const SCursorSprite& icon)
{
	if ( iconId >= (s32)Cursors.size() )
		return;

	for ( u32 i=0; i < Cursors[iconId].Frames.size(); ++i )
		DestroyCursor(Cursors[iconId].Frames[i].IconHW);

	if ( icon.SpriteId >= 0 )
	{
		CursorW32 cW32;
		cW32.FrameTime = icon.SpriteBank->getSprites()[icon.SpriteId].frameTime;
		for ( u32 i=0; i < icon.SpriteBank->getSprites()[icon.SpriteId].Frames.size(); ++i )
		{
			u32 texId = icon.SpriteBank->getSprites()[icon.SpriteId].Frames[i].textureNumber;
			u32 rectId = icon.SpriteBank->getSprites()[icon.SpriteId].Frames[i].rectNumber;
			rectangle<s32> rectIcon = icon.SpriteBank->getPositions()[rectId];

			HCURSOR hc = Device->TextureToCursor(HWnd, icon.SpriteBank->getTexture(texId), rectIcon, icon.HotSpot);
			cW32.Frames.push_back( CursorFrameW32(hc) );
		}

		Cursors[iconId] = cW32;
	}
}


//! Return a system-specific size which is supported for cursors. Larger icons will fail, smaller icons might work.
dimension2di CDreamDeviceWin32::CCursorControl::getSupportedIconSize() const
{
	dimension2di result;

	result.Width = GetSystemMetrics(SM_CXCURSOR);
	result.Height = GetSystemMetrics(SM_CYCURSOR);

	return result;
}

#endif // _DREAM_COMPILE_WITH_WINDOWS_DEVICE_
