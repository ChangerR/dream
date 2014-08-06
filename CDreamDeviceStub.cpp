// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CDreamDeviceStub.h"
#include "IFileSystem.h"
#include "os.h"
#include "dreamCompileConfig.h"
#include "CTimer.h"
#include "CLogger.h"
#include "dstring.h"
#include "IRandomizer.h"

//! constructor
CDreamDeviceStub::CDreamDeviceStub(const SDreamCreationParameters& params)
: DreamDevice(), VideoDriver(0),
	Timer(0), CursorControl(0),
	Logger(0), Operator(0), Randomizer(0), FileSystem(0), VideoModeList(0),
	CreationParams(params), Close(false)
{
	Timer = new CTimer(params.UsePerformanceTimer);
	if (Printer::Logger)
	{
		Printer::Logger->addRef();
		Logger = (CLogger*)Printer::Logger;
	}
	else
	{
		Logger = new CLogger();
		Printer::Logger = Logger;
	}
	Logger->setLogLevel(CreationParams.LoggingLevel);

	Printer::Logger = Logger;
	Randomizer = createDefaultRandomizer();

	FileSystem = createFileSystem();
	VideoModeList = new CVideoModeList();

	stringc s = "Dream Engine version ";
	s.append(getVersion());
	Printer::log(s.c_str(), ELL_INFORMATION);

	checkVersion(params.SDK_version_do_not_use);
}


CDreamDeviceStub::~CDreamDeviceStub()
{
	VideoModeList->releaseRef();
	FileSystem->releaseRef();
	if (VideoDriver)
		VideoDriver->releaseRef();

	if (CursorControl)
		CursorControl->releaseRef();

	if (Operator)
		Operator->releaseRef();

	if (Randomizer)
		Randomizer->releaseRef();

	CursorControl = 0;

	if (Timer)
		Timer->releaseRef();

	if (Logger->releaseRef())
		Printer::Logger = 0;
}


//! returns the video driver
IVideoDriver* CDreamDeviceStub::getVideoDriver()
{
	return VideoDriver;
}


//! return file system
IFileSystem* CDreamDeviceStub::getFileSystem()
{
	return FileSystem;
}

//! \return Returns a pointer to the ITimer object. With it the
//! current Time can be received.
ITimer* CDreamDeviceStub::getTimer()
{
	return Timer;
}


//! Returns the version of the engine.
const char* CDreamDeviceStub::getVersion() const
{
	return DREAM_SDK_VERSION;
}

//! \return Returns a pointer to the mouse cursor control interface.
ICursorControl* CDreamDeviceStub::getCursorControl()
{
	return CursorControl;
}


//! \return Returns a pointer to a list with all video modes supported
//! by the gfx adapter.
IVideoModeList* CDreamDeviceStub::getVideoModeList()
{
	return VideoModeList;
}


//! checks version of sdk and prints warning if there might be a problem
bool CDreamDeviceStub::checkVersion(const char* version)
{
	if (strcmp(getVersion(), version))
	{
		stringc w;
		w = "Warning: The library version of the Irrlicht Engine (";
		w += getVersion();
		w += ") does not match the version the application was compiled with (";
		w += version;
		w += "). This may cause problems.";
		Printer::log(w.c_str(), ELL_WARNING);
		return false;
	}

	return true;
}

//! \return Returns a pointer to the logger.
ILogger* CDreamDeviceStub::getLogger()
{
	return Logger;
}


//! Returns the operation system opertator object.
IOSOperator* CDreamDeviceStub::getOSOperator()
{
	return Operator;
}


//! Provides access to the engine's currently set randomizer.
IRandomizer* CDreamDeviceStub::getRandomizer() const
{
	return Randomizer;
}

//! Sets a new randomizer.
void CDreamDeviceStub::setRandomizer(IRandomizer* r)
{
	if (r!=Randomizer)
	{
		if (Randomizer)
			Randomizer->releaseRef();
		Randomizer=r;
		if (Randomizer)
			Randomizer->addRef();
	}
}

struct SDefaultRandomizer : public IRandomizer
{
	virtual void reset(s32 value=0x0f0f0f0f)
	{
		Randomizer::reset(value);
	}

	virtual s32 rand() const
	{
		return Randomizer::rand();
	}

	virtual f32 frand() const
	{
		return Randomizer::frand();
	}

	virtual s32 randMax() const
	{
		return Randomizer::randMax();
	}
};


//! Creates a new default randomizer.
IRandomizer* CDreamDeviceStub::createDefaultRandomizer() const
{
	IRandomizer* r = new SDefaultRandomizer();
	if (r)
		r->reset();
	return r;
}


//! Checks if the window is running in fullscreen mode
bool CDreamDeviceStub::isFullscreen() const
{
	return CreationParams.Fullscreen;
}


//! returns color format
ECOLOR_FORMAT CDreamDeviceStub::getColorFormat() const
{
	return ECF_R5G6B5;
}

/*!
*/
void CDreamDeviceStub::calculateGammaRamp ( u16 *ramp, f32 gamma, f32 relativebrightness, f32 relativecontrast )
{
	s32 i;
	s32 value;
	s32 rbright = (s32) ( relativebrightness * (65535.f / 4 ) );
	f32 rcontrast = 1.f / (255.f - ( relativecontrast * 127.5f ) );

	gamma = gamma > 0.f ? 1.0f / gamma : 0.f;

	for ( i = 0; i < 256; ++i )
	{
		value = (s32)(pow( rcontrast * i, gamma)*65535.f + 0.5f );
		ramp[i] = (u16) s32_clamp ( value + rbright, 0, 65535 );
	}

}

void CDreamDeviceStub::calculateGammaFromRamp ( f32 &gamma, const u16 *ramp )
{
	/* The following is adapted from a post by Garrett Bass on OpenGL
	Gamedev list, March 4, 2000.
	*/
	f32 sum = 0.0;
	s32 i, count = 0;

	gamma = 1.0;
	for ( i = 1; i < 256; ++i ) {
		if ( (ramp[i] != 0) && (ramp[i] != 65535) ) {
			f32 B = (f32)i / 256.f;
			f32 A = ramp[i] / 65535.f;
			sum += (f32) ( logf(A) / logf(B) );
			count++;
		}
	}
	if ( count && sum ) {
		gamma = 1.0f / (sum / count);
	}

}

//! Set the current Gamma Value for the Display
bool CDreamDeviceStub::setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast )
{
	return false;
}

//! Get the current Gamma Value for the Display
bool CDreamDeviceStub::getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast )
{
	return false;
}

//! Remove all messages pending in the system message loop
void CDreamDeviceStub::clearSystemMessages()
{
}


