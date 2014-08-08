// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Dreamlicht Engine".
// For conditions of distribution and use, see copyright notice in Dreamlicht.h

#ifndef __C_Dream_DEVICE_WIN32_H_INCLUDED__
#define __C_Dream_DEVICE_WIN32_H_INCLUDED__

#include "dreamCompileConfig.h"
#ifdef _DREAM_COMPILE_WITH_WINDOWS_DEVICE_
#include <Windows.h>
#include "CDreamDeviceStub.h"
#include "DreamDevice.h"
#include "IImagePresenter.h"

#define WIN32_LEAN_AND_MEAN
#if !defined(GET_X_LPARAM)
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif
class CDreamDeviceWin32 : public CDreamDeviceStub, IImagePresenter
{
public:

	//! constructor
	CDreamDeviceWin32(const SDreamCreationParameters& params);

	//! destructor
	virtual ~CDreamDeviceWin32();

	//! runs the device. Returns false if device wants to be deleted
	virtual bool run();

	//! Cause the device to temporarily pause execution and let other processes to run
	// This should bring down processor usage without major performance loss for Dreamlicht
	virtual void yield();

	//! Pause execution and let other processes to run for a specified amount of time.
	virtual void sleep(u32 timeMs, bool pauseTimer);

	//! sets the caption of the window
	virtual void setWindowCaption(const wchar_t* text);

	//! returns if window is active. if not, nothing need to be drawn
	virtual bool isWindowActive() const;

	//! returns if window has focus
	virtual bool isWindowFocused() const;

	//! returns if window is minimized
	virtual bool isWindowMinimized() const;

	//! presents a surface in the client area
	virtual bool present(IImage* surface, void* windowId=0, rectangle<s32>* src=0);

	//! notifies the device that it should close itself
	virtual void closeDevice();

	//! \return Returns a pointer to a list with all video modes
	//! supported by the gfx adapter.
	IVideoModeList* getVideoModeList();

	//! Notifies the device, that it has been resized
	void OnResized();

	//! Sets if the window should be resizable in windowed mode.
	virtual void setResizable(bool resize=false);

	//! Minimizes the window.
	virtual void minimizeWindow();

	//! Maximizes the window.
	virtual void maximizeWindow();

	//! Restores the window size.
	virtual void restoreWindow();

	//! Set the current Gamma Value for the Display
	virtual bool setGammaRamp( f32 red, f32 green, f32 blue, f32 brightness, f32 contrast );

	//! Get the current Gamma Value for the Display
	virtual bool getGammaRamp( f32 &red, f32 &green, f32 &blue, f32 &brightness, f32 &contrast );

	//! Remove all messages pending in the system message loop
	virtual void clearSystemMessages();

	//! Get the device type
	virtual E_DEVICE_TYPE getType() const
	{
			return EIDT_WIN32;
	}

	//! switchs to fullscreen
	bool switchToFullScreen(bool reset=false);

	//! Check for and show last Windows API error to help internal debugging.
	//! Does call GetLastError and on errors formats the errortext and displays it in a messagebox.
	static void ReportLastWinApiError();

	// convert an Dreamlicht texture to a windows cursor
	HCURSOR TextureToCursor(HWND hwnd, ITexture * tex, const rectangle<s32>& sourceRect, const position2d<s32> &hotspot);

	//! Implementation of the win32 cursor control
	class CCursorControl : public ICursorControl
	{
	public:

		CCursorControl(CDreamDeviceWin32* device, const dimension2d<u32>& wsize, HWND hwnd, bool fullscreen);
		~CCursorControl();

		//! Changes the visible state of the mouse cursor.
		virtual void setVisible(bool visible)
		{
			CURSORINFO info;
			info.cbSize = sizeof(CURSORINFO);
			BOOL gotCursorInfo = GetCursorInfo(&info);
			while ( gotCursorInfo )
			{
#ifdef CURSOR_SUPPRESSED
				// new flag for Windows 8, where cursor
				// might be suppressed for touch interface
				if (info.flags == CURSOR_SUPPRESSED)
				{
					visible=false;
					break;
				}
#endif
				if ( (visible && info.flags == CURSOR_SHOWING) || // visible
					(!visible && info.flags == 0 ) ) // hidden
				{
					break;
				}
				// this only increases an internal
				// display counter in windows, so it
				// might have to be called some more
				const int showResult = ShowCursor(visible);
				// if result has correct sign we can
				// stop here as well
				if (( !visible && showResult < 0 ) ||
					(visible && showResult >= 0))
					break;
				// yes, it really must be set each time
				info.cbSize = sizeof(CURSORINFO);
				gotCursorInfo = GetCursorInfo(&info);
			}
			IsVisible = visible;
		}

		//! Returns if the cursor is currently visible.
		virtual bool isVisible() const
		{
			return IsVisible;
		}

		//! Sets the new position of the cursor.
		virtual void setPosition(const position2d<f32> &pos)
		{
			setPosition(pos.X, pos.Y);
		}

		//! Sets the new position of the cursor.
		virtual void setPosition(f32 x, f32 y)
		{
			if (!UseReferenceRect)
				setPosition(round32(x*WindowSize.Width), round32(y*WindowSize.Height));
			else
				setPosition(round32(x*ReferenceRect.getWidth()), round32(y*ReferenceRect.getHeight()));
		}

		//! Sets the new position of the cursor.
		virtual void setPosition(const position2d<s32> &pos)
		{
			setPosition(pos.X, pos.Y);
		}

		//! Sets the new position of the cursor.
		virtual void setPosition(s32 x, s32 y)
		{
			if (UseReferenceRect)
			{
				SetCursorPos(ReferenceRect.UpperLeftCorner.X + x,
							 ReferenceRect.UpperLeftCorner.Y + y);
			}
			else
			{
				RECT rect;
				if (GetWindowRect(HWnd, &rect))
					SetCursorPos(x + rect.left + BorderX, y + rect.top + BorderY);
			}

			CursorPos.X = x;
			CursorPos.Y = y;
		}

		//! Returns the current position of the mouse cursor.
		virtual const position2d<s32>& getPosition()
		{
			updateInternalCursorPosition();
			return CursorPos;
		}

		//! Returns the current position of the mouse cursor.
		virtual position2d<f32> getRelativePosition()
		{
			updateInternalCursorPosition();

			if (!UseReferenceRect)
			{
				return position2d<f32>(CursorPos.X * InvWindowSize.Width,
					CursorPos.Y * InvWindowSize.Height);
			}

			return position2d<f32>(CursorPos.X / (f32)ReferenceRect.getWidth(),
					CursorPos.Y / (f32)ReferenceRect.getHeight());
		}

		//! Sets an absolute reference rect for calculating the cursor position.
		virtual void setReferenceRect(rectangle<s32>* rect=0)
		{
			if (rect)
			{
				ReferenceRect = *rect;
				UseReferenceRect = true;

				// prevent division through zero and uneven sizes

				if (!ReferenceRect.getHeight() || ReferenceRect.getHeight()%2)
					ReferenceRect.LowerRightCorner.Y += 1;

				if (!ReferenceRect.getWidth() || ReferenceRect.getWidth()%2)
					ReferenceRect.LowerRightCorner.X += 1;
			}
			else
				UseReferenceRect = false;
		}

		/** Used to notify the cursor that the window was resized. */
		virtual void OnResize(const dimension2d<u32>& size)
		{
			WindowSize = size;
			if (size.Width!=0)
				InvWindowSize.Width = 1.0f / size.Width;
			else
				InvWindowSize.Width = 0.f;

			if (size.Height!=0)
				InvWindowSize.Height = 1.0f / size.Height;
			else
				InvWindowSize.Height = 0.f;
		}

		/** Used to notify the cursor that the window resizable settings changed. */
		void updateBorderSize(bool fullscreen, bool resizable)
		{
		   if (!fullscreen)
		   {
			  if (resizable)
			  {
				 BorderX = GetSystemMetrics(SM_CXSIZEFRAME);
				 BorderY = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYSIZEFRAME);
			  }
			  else
			  {
				 BorderX = GetSystemMetrics(SM_CXDLGFRAME);
				 BorderY = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYDLGFRAME);
			  }
		   }
		   else
		   {
			  BorderX = BorderY = 0;
		   }
		}


		//! Sets the active cursor icon
		virtual void setActiveIcon(ECURSOR_ICON iconId);

		//! Gets the currently active icon
		virtual ECURSOR_ICON getActiveIcon() const
		{
			return ActiveIcon;
		}

		//! Add a custom sprite as cursor icon.
		virtual ECURSOR_ICON addIcon(const SCursorSprite& icon);

		//! replace the given cursor icon.
		virtual void changeIcon(ECURSOR_ICON iconId, const SCursorSprite& icon);

		//! Return a system-specific size which is supported for cursors. Larger icons will fail, smaller icons might work.
		virtual dimension2di getSupportedIconSize() const;

		void update();

	private:

		//! Updates the internal cursor position
		void updateInternalCursorPosition()
		{
			POINT p;
			if (!GetCursorPos(&p))
			{
				DWORD xy = GetMessagePos();
				p.x = GET_X_LPARAM(xy);
				p.y = GET_Y_LPARAM(xy);
			}

			if (UseReferenceRect)
			{
				CursorPos.X = p.x - ReferenceRect.UpperLeftCorner.X;
				CursorPos.Y = p.y - ReferenceRect.UpperLeftCorner.Y;
			}
			else
			{
				RECT rect;
				if (GetWindowRect(HWnd, &rect))
				{
					CursorPos.X = p.x-rect.left-BorderX;
					CursorPos.Y = p.y-rect.top-BorderY;
				}
				else
				{
					// window seems not to be existent, so set cursor to
					// a negative value
					CursorPos.X = -1;
					CursorPos.Y = -1;
				}
			}
		}

		CDreamDeviceWin32* Device;
		position2d<s32> CursorPos;
		dimension2d<u32> WindowSize;
		dimension2d<f32> InvWindowSize;
		HWND HWnd;

		s32 BorderX, BorderY;
		rectangle<s32> ReferenceRect;
		bool UseReferenceRect;
		bool IsVisible;


		struct CursorFrameW32
		{
			CursorFrameW32() : IconHW(0) {}
			CursorFrameW32(HCURSOR icon) : IconHW(icon) {}

			HCURSOR IconHW;	// hardware cursor
		};

		struct CursorW32
		{
			CursorW32() {}
			explicit CursorW32(HCURSOR iconHw, u32 frameTime=0) : FrameTime(frameTime)
			{
				Frames.push_back( CursorFrameW32(iconHw) );
			}
			array<CursorFrameW32> Frames;
			u32 FrameTime;
		};

		array<CursorW32> Cursors;
		ECURSOR_ICON ActiveIcon;
		u32 ActiveIconStartTime;

		void initCursors();
	};

	//! returns the win32 cursor control
	CCursorControl* getWin32CursorControl();

private:

	//! create the driver
	void createDriver();

	//! Process system events
	void handleSystemMessages();

	void getWindowsVersion(stringc& version);

	void resizeIfNecessary();

	HWND HWnd;

	bool ChangedToFullScreen;
	bool Resized;
	bool ExternalWindow;
	CCursorControl* Win32CursorControl;
	DEVMODE DesktopMode;
};

#endif // _Dream_COMPILE_WITH_WINDOWS_DEVICE_
#endif // __C_Dream_DEVICE_WIN32_H_INCLUDED__
