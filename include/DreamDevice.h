// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "DREAMlicht Engine".
// For conditions of distribution and use, see copyright notice in DREAMlicht.h

#ifndef __I_DREAM_DEVICE_H_INCLUDED__
#define __I_DREAM_DEVICE_H_INCLUDED__

#include "IReferenceCounted.h"
#include "dimension2d.h"
#include "IVideoDriver.h"
#include "EDriverTypes.h"
#include "EDeviceTypes.h"
#include "ICursorControl.h"
#include "IVideoModeList.h"
#include "ITimer.h"
#include "IOSOperator.h"

class ILogger;
class IRandomizer;
class IFileSystem;


class DreamDevice : public virtual IReferenceCounted
{
public:

	//! Runs the device.
	/** Also increments the virtual timer by calling
	ITimer::tick();. You can prevent this
	by calling ITimer::stop(); before and ITimer::start() after
	calling DreamDevice::run(). Returns false if device wants
	to be deleted. Use it in this way:
	\code
	while(device->run())
	{
		// draw everything here
	}
	\endcode
	If you want the device to do nothing if the window is inactive
	(recommended), use the slightly enhanced code shown at isWindowActive().

	Note if you are running DREAMlicht inside an external, custom
	created window: Calling Device->run() will cause DREAMlicht to
	dispatch windows messages internally.
	If you are running DREAMlicht in your own custom window, you can
	also simply use your own message loop using GetMessage,
	DispatchMessage and whatever and simply don't use this method.
	But note that DREAMlicht will not be able to fetch user input
	then. See DREAM::SDREAMlichtCreationParameters::WindowId for more
	informations and example code.
	*/
	virtual bool run() = 0;

	//! Cause the device to temporarily pause execution and let other processes run.
	/** This should bring down processor usage without major
	performance loss for DREAMlicht */
	virtual void yield() = 0;

	//! Pause execution and let other processes to run for a specified amount of time.
	/** It may not wait the full given time, as sleep may be interrupted
	\param timeMs: Time to sleep for in milisecs.
	\param pauseTimer: If true, pauses the device timer while sleeping
	*/
	virtual void sleep(u32 timeMs, bool pauseTimer=false) = 0;

	//! Provides access to the video driver for drawing 3d and 2d geometry.
	/** \return Pointer the video driver. */
	virtual IVideoDriver* getVideoDriver() = 0;

	//! Provides access to the virtual file system.
	/** \return Pointer to the file system. */
	virtual IFileSystem* getFileSystem() = 0;

	//! Provides access to the cursor control.
	/** \return Pointer to the mouse cursor control interface. */
	virtual ICursorControl* getCursorControl() = 0;

	//! Provides access to the message logger.
	/** \return Pointer to the logger. */
	virtual ILogger* getLogger() = 0;

	//! Gets a list with all video modes available.
	/** If you are confused now, because you think you have to
	create an DREAMlicht Device with a video mode before being able
	to get the video mode list, let me tell you that there is no
	need to start up an DREAMlicht Device with EDT_DIRECT3D8,
	EDT_OPENGL or EDT_SOFTWARE: For this (and for lots of other
	reasons) the null driver, EDT_NULL exists.
	\return Pointer to a list with all video modes supported
	by the gfx adapter. */
	virtual IVideoModeList* getVideoModeList() = 0;

	//! Provides access to the operation system operator object.
	/** The OS operator provides methods for
	getting system specific informations and doing system
	specific operations, such as exchanging data with the clipboard
	or reading the operation system version.
	\return Pointer to the OS operator. */
	virtual IOSOperator* getOSOperator() = 0;

	//! Provides access to the engine's timer.
	/** The system time can be retrieved by it as
	well as the virtual time, which also can be manipulated.
	\return Pointer to the ITimer object. */
	virtual ITimer* getTimer() = 0;

	//! Provides access to the engine's currently set randomizer.
	/** \return Pointer to the IRandomizer object. */
	virtual IRandomizer* getRandomizer() const =0;

	//! Sets a new randomizer.
	/** \param r Pointer to the new IRandomizer object. This object is
	grab()'ed by the engine and will be released upon the next setRandomizer
	call or upon device destruction. */
	virtual void setRandomizer(IRandomizer* r) =0;

	//! Creates a new default randomizer.
	/** The default randomizer provides the random sequence known from previous
	DREAMlicht versions and is the initial randomizer set on device creation.
	\return Pointer to the default IRandomizer object. */
	virtual IRandomizer* createDefaultRandomizer() const =0;

	//! Sets the caption of the window.
	/** \param text: New text of the window caption. */
	virtual void setWindowCaption(const wchar_t* text) = 0;

	//! Returns if the window is active.
	/** If the window is inactive,
	nothing needs to be drawn. So if you don't want to draw anything
	when the window is inactive, create your drawing loop this way:
	\code
	while(device->run())
	{
		if (device->isWindowActive())
		{
			// draw everything here
		}
		else
			device->yield();
	}
	\endcode
	\return True if window is active. */
	virtual bool isWindowActive() const = 0;

	//! Checks if the DREAMlicht window has focus
	/** \return True if window has focus. */
	virtual bool isWindowFocused() const = 0;

	//! Checks if the DREAMlicht window is minimized
	/** \return True if window is minimized. */
	virtual bool isWindowMinimized() const = 0;

	//! Checks if the DREAMlicht window is running in fullscreen mode
	/** \return True if window is fullscreen. */
	virtual bool isFullscreen() const = 0;

	//! Get the current color format of the window
	/** \return Color format of the window. */
	virtual ECOLOR_FORMAT getColorFormat() const = 0;

	//! Notifies the device that it should close itself.
	/** DreamDevice::run() will always return false after closeDevice() was called. */
	virtual void closeDevice() = 0;

	//! Get the version of the engine.
	/** The returned string
	will look like this: "1.2.3" or this: "1.2".
	\return String which contains the version. */
	virtual const c8* getVersion() const = 0;

	
	/** The default is false. This method only works in windowed
	mode.
	\param resize Flag whether the window should be resizable. */
	virtual void setResizable(bool resize=false) = 0;

	//! Minimizes the window if possible.
	virtual void minimizeWindow() =0;

	//! Maximizes the window if possible.
	virtual void maximizeWindow() =0;

	//! Restore the window to normal size if possible.
	virtual void restoreWindow() =0;


	//! Set the current Gamma Value for the Display
	virtual bool setGammaRamp(f32 red, f32 green, f32 blue,
				f32 relativebrightness, f32 relativecontrast) =0;

	//! Get the current Gamma Value for the Display
	virtual bool getGammaRamp(f32 &red, f32 &green, f32 &blue,
				f32 &brightness, f32 &contrast) =0;

	//! Remove messages pending in the system message loop
	/** This function is usually used after messages have been buffered for a longer time, for example
	when loading a large scene. Clearing the message loop prevents that mouse- or buttonclicks which users
	have pressed in the meantime will now trigger unexpected actions in the gui. <br>
	So far the following messages are cleared:<br>
	Win32: All keyboard and mouse messages<br>
	Linux: All keyboard and mouse messages<br>
	All other devices are not yet supported here.<br>
	The function is still somewhat experimental, as the kind of messages we clear is based on just a few use-cases.
	If you think further messages should be cleared, or some messages should not be cleared here, then please tell us. */
	virtual void clearSystemMessages() = 0;

	//! Get the type of the device.
	/** This allows the user to check which windowing system is currently being
	used. */
	virtual E_DEVICE_TYPE getType() const = 0;

	//! Check if a driver type is supported by the engine.
	/** Even if true is returned the driver may not be available
	for a configuration requested when creating the device. */
	static bool isDriverSupported(E_DRIVER_TYPE driver)
	{
		switch (driver)
		{
			case EDT_NULL:
				return true;
			case EDT_SOFTWARE:
#ifdef _DREAM_COMPILE_WITH_SOFTWARE_
				return true;
#else
				return false;
#endif
			case EDT_BURNINGSVIDEO:
#ifdef _DREAM_COMPILE_WITH_BURNINGSVIDEO_
				return true;
#else
				return false;
#endif
			case EDT_DIRECT3D8:
#ifdef _DREAM_COMPILE_WITH_DIRECT3D_8_
				return true;
#else
				return false;
#endif
			case EDT_DIRECT3D9:
#ifdef _DREAM_COMPILE_WITH_DIRECT3D_9_
				return true;
#else
				return false;
#endif
			case EDT_OPENGL:
#ifdef _DREAM_COMPILE_WITH_OPENGL_
				return true;
#else
				return false;
#endif
			default:
				return false;
		}
	}
};

#endif

