// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef _C_IMAGE_WRITER_BMP_H_INCLUDED__
#define _C_IMAGE_WRITER_BMP_H_INCLUDED__

#include "dreamCompileConfig.h"

#ifdef _DREAM_COMPILE_WITH_BMP_WRITER_

#include "IImageWriter.h"

class CImageWriterBMP : public IImageWriter
{
public:
	//! constructor
	CImageWriterBMP();

	//! return true if this writer can write a file with the given extension
	virtual bool isAWriteableFileExtension(const path& filename) const;

	//! write image to file
	virtual bool writeImage(IWriteFile *file, IImage *image, u32 param) const;
};



#endif
#endif // _C_IMAGE_WRITER_BMP_H_INCLUDED__

