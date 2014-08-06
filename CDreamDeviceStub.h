// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_IRR_DEVICE_STUB_H_INCLUDED__
#define __C_IRR_DEVICE_STUB_H_INCLUDED__

#include "DreamDevice.h"
#include "IImagePresenter.h"
#include "SDreamCreationParameters.h"
#include "CVideoModeList.h"

// lots of prototypes:
class ILogger;
class CLogger;
class IRandomizer;


IFileSystem* createFileSystem();


IVideoDriver* createSoftwareDriver(const dimension2d<u32>& windowSize,
		bool fullscreen, IFileSystem* io,
		IImagePresenter* presenter);
IVideoDriver* createBurningVideoDriver(const SDreamCreationParameters& params,
		IFileSystem* io, IImagePresenter* presenter);
IVideoDriver* createNullDriver(IFileSystem* io, const dimension2d<u32>& screenSize);



//! Stub for an Irrlicht Device implementation
class CDreamDeviceStub : public DreamDevice
{
public:

	//! constructor
	CDreamDeviceStub(const SDreamCreationParameters& param);

	//! destructor
	virtual ~CDreamDeviceStub();

	//! returns the video driver
	virtual IVideoDriver* getVideoDriver();

	//! return file system
	virtual IFileSystem* getFileSystem();

	//! \return Returns a pointer to the mouse cursor control interface.
	virtual ICursorControl* getCursorControl();

	//! Returns a pointer to a list with all video modes supported by the gfx adapter.
	virtual IVideoModeList* getVideoModeList();

	//! Returns a pointer to the ITimer object. With it the current Time can be received.
	virtual ITimer* getTimer();

	//! Returns the version of the engine.
	virtual const char* getVersion() const;
	
	//! Returns a pointer to the logger.
	virtual ILogger* getLogger();

	//! Provides access to the engine's currently set randomizer.
	virtual IRandomizer* getRandomizer() const;

	//! Sets a new randomizer.
	virtual void setRandomizer(IRandomizer* r);

	//! Creates a new default randomizer.
	virtual IRandomizer* createDefaultRandomizer() const;

	//! Returns the operation system opertator object.
	virtual IOSOperator* getOSOperator();

	//! Checks if the window is running in fullscreen mode.
	virtual bool isFullscreen() const;

	//! get color format of the current window
	virtual ECOLOR_FORMAT getColorFormat() const;

	//! Set the current Gamma Value for the Display
	virtual bool setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast );

	//! Get the current Gamma Value for the Display
	virtual bool getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast );

	//! Remove all messages pending in the system message loop
	virtual void clearSystemMessages();


protected:

	//! checks version of SDK and prints warning if there might be a problem
	bool checkVersion(const char* version);

	void calculateGammaRamp ( u16 *ramp, f32 gamma, f32 relativebrightness, f32 relativecontrast );
	void calculateGammaFromRamp ( f32 &gamma, const u16 *ramp );

	IVideoDriver* VideoDriver;
	ITimer* Timer;
	ICursorControl* CursorControl;
	CLogger* Logger;
	IOSOperator* Operator;
	IRandomizer* Randomizer;
	IFileSystem* FileSystem;
	CVideoModeList* VideoModeList;
	SDreamCreationParameters CreationParams;
	bool Close;
};


#endif

