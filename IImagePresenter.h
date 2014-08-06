// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_IMAGE_PRESENTER_H_INCLUDED__
#define __I_IMAGE_PRESENTER_H_INCLUDED__

#include "IImage.h"


/*!
	Interface for a class which is able to present an IImage 
	an the Screen. Usually only implemented by an IrrDevice for
	presenting Software Device Rendered images.

	This class should be used only internally.
*/

class IImagePresenter
{
public:

	virtual ~IImagePresenter() {};
	//! presents a surface in the client area
	virtual bool present(IImage* surface, void* windowId=0,rectangle<s32>* src=0 ) = 0;
};


#endif

