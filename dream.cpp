// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "dreamCompileConfig.h"

static const char* const copyright = "Dream Engine";

#ifdef _DREAM_WINDOWS
	#include <windows.h>
	#if defined(_DEBUG)
		#include <crtdbg.h>
	#endif // _DEBUG
#endif

#ifdef _DREAM_COMPILE_WITH_WINDOWS_DEVICE_
#include "CDreamDeviceWin32.h"
#endif
#include "SDreamCreationParameters.h"
#include "IVideoDriver.h"
DreamDevice* createDeviceEx(const SDreamCreationParameters& params)
{
	DreamDevice* dev = 0;

#ifdef _DREAM_COMPILE_WITH_WINDOWS_DEVICE_
	if (params.DeviceType == EIDT_WIN32 || (!dev && params.DeviceType == EIDT_BEST))
		dev = new CDreamDeviceWin32(params);
#endif
	if (dev && !dev->getVideoDriver() && params.DriverType != EDT_NULL)
	{
		dev->closeDevice(); // destroy window
		dev->run(); // consume quit message
		dev->releaseRef();
		dev = 0;
	}

	return dev;
}
//! stub for calling createDeviceEx
DreamDevice* createDevice(E_DRIVER_TYPE driverType,
		const dimension2d<u32>& windowSize,
		u32 bits, bool fullscreen,
		bool stencilbuffer, bool vsync)
{
	SDreamCreationParameters p;
	p.DriverType = driverType;
	p.WindowSize = windowSize;
	p.Bits = (u8)bits;
	p.Fullscreen = fullscreen;
	p.Stencilbuffer = stencilbuffer;
	p.Vsync = vsync;

	return createDeviceEx(p);
}

const matrix4 IdentityMatrix(matrix4::EM4CONST_IDENTITY);
stringc LOCALE_DECIMAL_POINTS(".");
SMaterial IdentityMaterial;

int WINAPI WinMain(  
                   HINSTANCE hInstance,       // handle to current instance  
                   HINSTANCE hPrevInstance,      // handle to previous instance  
                   LPSTR lpCmdLine,           // command line  
                   int nCmdShow                    // show state  
                   ) 
{
	DreamDevice *device =createDevice( EDT_DIRECT3D9, dimension2d<u32>(640, 480), 32,false, false, false);
	if (!device)
		return 1;
	device->setWindowCaption(L"Hello World!");
	IVideoDriver* driver = device->getVideoDriver();
	if(!driver) 
		return 1;
	while(device->run())
	{
		driver->beginScene(true, true, SColor(255,100,101,140));
		driver->endScene();
		device->sleep(10);
	}
	device->releaseRef();
	return 0;
}

