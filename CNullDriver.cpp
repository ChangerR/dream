// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CNullDriver.h"
#include "os.h"
#include "CImage.h"
#include "CAttributes.h"
#include "IReadFile.h"
#include "IWriteFile.h"
#include "IImageLoader.h"
#include "IImageWriter.h"
#include "CColorConverter.h"
#include <stdio.h>
#include "IMaterialRenderer.h"
//! creates a loader which is able to load windows bitmaps
IImageLoader* createImageLoaderBMP();
IImageWriter* createImageWriterBMP();
//! constructor
CNullDriver::CNullDriver(IFileSystem* io, const dimension2d<u32>& screenSize)
: FileSystem(io), ViewPort(0,0,0,0), ScreenSize(screenSize),
	PrimitivesDrawn(0), MinVertexCountForVBO(500), TextureCreationFlags(0),
	OverrideMaterial2DEnabled(false), AllowZWriteOnTransparent(false)
{
	DriverAttributes = new CAttributes();
	DriverAttributes->addInt("MaxTextures", _DREAM_MATERIAL_MAX_TEXTURES_);
	DriverAttributes->addInt("MaxSupportedTextures", _DREAM_MATERIAL_MAX_TEXTURES_);
	DriverAttributes->addInt("MaxLights", getMaximalDynamicLightAmount());
	DriverAttributes->addInt("MaxAnisotropy", 1);
//	DriverAttributes->addInt("MaxUserClipPlanes", 0);
//	DriverAttributes->addInt("MaxAuxBuffers", 0);
	DriverAttributes->addInt("MaxMultipleRenderTargets", 1);
	DriverAttributes->addInt("MaxIndices", -1);
	DriverAttributes->addInt("MaxTextureSize", -1);
//	DriverAttributes->addInt("MaxGeometryVerticesOut", 0);
//	DriverAttributes->addFloat("MaxTextureLODBias", 0.f);
	DriverAttributes->addInt("Version", 1);
//	DriverAttributes->addInt("ShaderLanguageVersion", 0);
//	DriverAttributes->addInt("AntiAlias", 0);

	setFog();

	setTextureCreationFlag(ETCF_ALWAYS_32_BIT, true);
	setTextureCreationFlag(ETCF_CREATE_MIP_MAPS, true);

	ViewPort = rectangle<s32>(position2d<s32>(0,0), dimension2di(screenSize));

	if (FileSystem)
		FileSystem->addRef();

	// create surface loader
#ifdef _DREAM_COMPILE_WITH_BMP_LOADER_
	SurfaceLoader.push_back(createImageLoaderBMP());
#endif
#ifdef _DREAM_COMPILE_WITH_BMP_WRITER_
	SurfaceWriter.push_back(createImageWriterBMP());
#endif


	// set ExposedData to 0
	memset(&ExposedData, 0, sizeof(ExposedData));
	for (u32 i=0; i<EVDF_COUNT; ++i)
		FeatureEnabled[i]=true;

	InitMaterial2D.AntiAliasing=EAAM_OFF;
	InitMaterial2D.Lighting=false;
	InitMaterial2D.ZWriteEnable=false;
	InitMaterial2D.ZBuffer=ECFN_NEVER;
	InitMaterial2D.UseMipMaps=false;
	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	{
		InitMaterial2D.TextureLayer[i].BilinearFilter=false;
		InitMaterial2D.TextureLayer[i].TextureWrapU=ETC_REPEAT;
		InitMaterial2D.TextureLayer[i].TextureWrapV=ETC_REPEAT;
	}
	OverrideMaterial2D=InitMaterial2D;
}


//! destructor
CNullDriver::~CNullDriver()
{
	if (DriverAttributes)
		DriverAttributes->releaseRef();

	if (FileSystem)
		FileSystem->releaseRef();
		
	deleteAllTextures();

	u32 i;
	for (i=0; i<SurfaceLoader.size(); ++i)
		SurfaceLoader[i]->releaseRef();

	for (i=0; i<SurfaceWriter.size(); ++i)
		SurfaceWriter[i]->releaseRef();

	// delete material renderers
	deleteMaterialRenders();
}


//! Adds an external surface loader to the engine.
void CNullDriver::addExternalImageLoader(IImageLoader* loader)
{
	if (!loader)
		return;

	loader->addRef();
	SurfaceLoader.push_back(loader);
}


//! Adds an external surface writer to the engine.
void CNullDriver::addExternalImageWriter(IImageWriter* writer)
{
	if (!writer)
		return;

	writer->addRef();
	SurfaceWriter.push_back(writer);
}


//! Retrieve the number of image loaders
u32 CNullDriver::getImageLoaderCount() const
{
	return SurfaceLoader.size();
}


//! Retrieve the given image loader
IImageLoader* CNullDriver::getImageLoader(u32 n)
{
	if (n < SurfaceLoader.size())
		return SurfaceLoader[n];
	return 0;
}


//! Retrieve the number of image writers
u32 CNullDriver::getImageWriterCount() const
{
	return SurfaceWriter.size();
}


//! Retrieve the given image writer
IImageWriter* CNullDriver::getImageWriter(u32 n)
{
	if (n < SurfaceWriter.size())
		return SurfaceWriter[n];
	return 0;
}


//! deletes all textures
void CNullDriver::deleteAllTextures()
{
	// we need to remove previously set textures which might otherwise be kept in the
	// last set material member. Could be optimized to reduce state changes.
	setMaterial(SMaterial());

	for (u32 i=0; i<Textures.size(); ++i)
		Textures[i].Surface->releaseRef();

	Textures.clear();
}



//! applications must call this method before performing any rendering. returns false if failed.
bool CNullDriver::beginScene(bool backBuffer, bool zBuffer, SColor color,
		const SExposedVideoData& videoData, rectangle<s32>* sourceRect)
{
	clearFPUException();
	PrimitivesDrawn = 0;
	return true;
}


//! applications must call this method after performing any rendering. returns false if failed.
bool CNullDriver::endScene()
{
	FPSCounter.registerFrame(Timer::getRealTime(), PrimitivesDrawn);
	return true;
}


//! Disable a feature of the driver.
void CNullDriver::disableFeature(E_VIDEO_DRIVER_FEATURE feature, bool flag)
{
	FeatureEnabled[feature]=!flag;
}


//! queries the features of the driver, returns true if feature is available
bool CNullDriver::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
{
	return false;
}


//! Get attributes of the actual video driver
const IAttributes& CNullDriver::getDriverAttributes() const
{
	return *DriverAttributes;
}


//! sets transformation
void CNullDriver::setTransform(E_TRANSFORMATION_STATE state, const matrix4& mat)
{
}


//! Returns the transformation set by setTransform
const matrix4& CNullDriver::getTransform(E_TRANSFORMATION_STATE state) const
{
	return TransformationMatrix;
}


//! sets a material
void CNullDriver::setMaterial(const SMaterial& material)
{
}


//! Removes a texture from the texture cache and deletes it, freeing lot of
//! memory.
void CNullDriver::removeTexture(ITexture* texture)
{
	if (!texture)
		return;

	for (u32 i=0; i<Textures.size(); ++i)
	{
		if (Textures[i].Surface == texture)
		{
			texture->releaseRef();
			Textures.erase(i);
		}
	}
}


//! Removes all texture from the texture cache and deletes them, freeing lot of
//! memory.
void CNullDriver::removeAllTextures()
{
	setMaterial ( SMaterial() );
	deleteAllTextures();
}


//! Returns a texture by index
ITexture* CNullDriver::getTextureByIndex(u32 i)
{
	if ( i < Textures.size() )
		return Textures[i].Surface;

	return 0;
}


//! Returns amount of textures currently loaded
u32 CNullDriver::getTextureCount() const
{
	return Textures.size();
}


//! Renames a texture
void CNullDriver::renameTexture(ITexture* texture, const path& newName)
{
	// we can do a const_cast here safely, the name of the ITexture interface
	// is just readonly to prevent the user changing the texture name without invoking
	// this method, because the textures will need resorting afterwards

	SNamedPath& name = const_cast<SNamedPath&>(texture->getName());
	name.setPath(newName);

	Textures.sort();
}


//! loads a Texture
ITexture* CNullDriver::getTexture(const path& filename)
{
	// Identify textures by their absolute filenames if possible.
	const path absolutePath = FileSystem->getAbsolutePath(filename);

	ITexture* texture = findTexture(absolutePath);
	if (texture)
		return texture;

	// Then try the raw filename, which might be in an Archive
	texture = findTexture(filename);
	if (texture)
		return texture;

	// Now try to open the file using the complete path.
	IReadFile* file = FileSystem->createAndOpenFile(absolutePath);

	if (!file)
	{
		// Try to open it using the raw filename.
		file = FileSystem->createAndOpenFile(filename);
	}

	if (file)
	{
		// Re-check name for actual archive names
		texture = findTexture(file->getFileName());
		if (texture)
		{
			file->releaseRef();
			return texture;
		}

		texture = loadTextureFromFile(file);
		file->releaseRef();

		if (texture)
		{
			addTexture(texture);
			texture->releaseRef(); // drop it because we created it, one grab too much
		}
		else
			Printer::log("Could not load texture", filename, ELL_ERROR);
		return texture;
	}
	else
	{
		Printer::log("Could not open file of texture", filename, ELL_WARNING);
		return 0;
	}
}


//! loads a Texture
ITexture* CNullDriver::getTexture(IReadFile* file)
{
	ITexture* texture = 0;

	if (file)
	{
		texture = findTexture(file->getFileName());

		if (texture)
			return texture;

		texture = loadTextureFromFile(file);

		if (texture)
		{
			addTexture(texture);
			texture->releaseRef(); // drop it because we created it, one grab too much
		}

		if (!texture)
			Printer::log("Could not load texture", file->getFileName(), ELL_WARNING);
	}

	return texture;
}


//! opens the file and loads it into the surface
ITexture* CNullDriver::loadTextureFromFile(IReadFile* file, const path& hashName )
{
	ITexture* texture = 0;
	IImage* image = createImageFromFile(file);

	if (image)
	{
		// create texture from surface
		texture = createDeviceDependentTexture(image, hashName.size() ? hashName : file->getFileName() );
		Printer::log("Loaded texture", file->getFileName());
		image->releaseRef();
	}

	return texture;
}


//! adds a surface, not loaded or created by the Irrlicht Engine
void CNullDriver::addTexture(ITexture* texture)
{
	if (texture)
	{
		SSurface s;
		s.Surface = texture;
		texture->addRef();

		Textures.push_back(s);

		// the new texture is now at the end of the texture list. when searching for
		// the next new texture, the texture array will be sorted and the index of this texture
		// will be changed. to let the order be more consistent to the user, sort
		// the textures now already although this isn't necessary:

		Textures.sort();
	}
}


//! looks if the image is already loaded
ITexture* CNullDriver::findTexture(const path& filename)
{
	SSurface s;
	SDummyTexture dummy(filename);
	s.Surface = &dummy;

	s32 index = Textures.binary_search(s);
	if (index != -1)
		return Textures[index].Surface;

	return 0;
}


//! Creates a texture from a loaded IImage.
ITexture* CNullDriver::addTexture(const path& name, IImage* image, void* mipmapData)
{
	if ( 0 == name.size() || !image)
		return 0;

	ITexture* t = createDeviceDependentTexture(image, name, mipmapData);
	if (t)
	{
		addTexture(t);
		t->releaseRef();
	}
	return t;
}


//! creates a Texture
ITexture* CNullDriver::addTexture(const dimension2d<u32>& size,
				  const path& name, ECOLOR_FORMAT format)
{
	if(IImage::isRenderTargetOnlyFormat(format))
	{
		Printer::log("Could not create ITexture, format only supported for render target textures.", ELL_WARNING);
		return 0;
	}

	if ( 0 == name.size () )
		return 0;

	IImage* image = new CImage(format, size);
	ITexture* t = createDeviceDependentTexture(image, name);
	image->releaseRef();
	addTexture(t);

	if (t)
		t->releaseRef();

	return t;
}



//! returns a device dependent texture from a software surface (IImage)
//! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
ITexture* CNullDriver::createDeviceDependentTexture(IImage* surface, const path& name, void* mipmapData)
{
	return new SDummyTexture(name);
}


//! set or reset special render targets
bool CNullDriver::setRenderTarget(E_RENDER_TARGET target, bool clearTarget,
			bool clearZBuffer, SColor color)
{
	if (ERT_FRAME_BUFFER==target)
		return setRenderTarget(0,clearTarget, clearZBuffer, color);
	else
		return false;
}


//! sets a render target
bool CNullDriver::setRenderTarget(ITexture* texture, bool clearBackBuffer,
					bool clearZBuffer, SColor color)
{
	return false;
}


//! Sets multiple render targets
bool CNullDriver::setRenderTarget(const array<IRenderTarget>& texture,
				bool clearBackBuffer, bool clearZBuffer, SColor color)
{
	return false;
}


//! sets a viewport
void CNullDriver::setViewPort(const rectangle<s32>& area)
{
}


//! gets the area of the current viewport
const rectangle<s32>& CNullDriver::getViewPort() const
{
	return ViewPort;
}


//! draws a vertex primitive list
void CNullDriver::drawVertexPrimitiveList(const void* vertices, u32 vertexCount, const void* indexList, u32 primitiveCount, E_VERTEX_TYPE vType, E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if ((iType==EIT_16BIT) && (vertexCount>65536))
		Printer::log("Too many vertices for 16bit index type, render artifacts may occur.");
	PrimitivesDrawn += primitiveCount;
}


//! draws a vertex primitive list in 2d
void CNullDriver::draw2DVertexPrimitiveList(const void* vertices, u32 vertexCount, const void* indexList, u32 primitiveCount, E_VERTEX_TYPE vType, E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType)
{
	if ((iType==EIT_16BIT) && (vertexCount>65536))
		Printer::log("Too many vertices for 16bit index type, render artifacts may occur.");
	PrimitivesDrawn += primitiveCount;
}


//! Draws a 3d line.
void CNullDriver::draw3DLine(const vector3df& start,
				const vector3df& end, SColor color)
{
}


//! Draws a 3d triangle.
void CNullDriver::draw3DTriangle(const triangle3df& triangle, SColor color)
{
	S3DVertex vertices[3];
	vertices[0].Pos=triangle.pointA;
	vertices[0].Color=color;
	vertices[0].Normal=triangle.getNormal().normalize();
	vertices[0].TCoords.set(0.f,0.f);
	vertices[1].Pos=triangle.pointB;
	vertices[1].Color=color;
	vertices[1].Normal=vertices[0].Normal;
	vertices[1].TCoords.set(0.5f,1.f);
	vertices[2].Pos=triangle.pointC;
	vertices[2].Color=color;
	vertices[2].Normal=vertices[0].Normal;
	vertices[2].TCoords.set(1.f,0.f);
	const u16 indexList[] = {0,1,2};
	drawVertexPrimitiveList(vertices, 3, indexList, 1, EVT_STANDARD, EPT_TRIANGLES, EIT_16BIT);
}


//! Draws a 3d axis aligned box.
void CNullDriver::draw3DBox(const aabbox3d<f32>& box, SColor color)
{
	vector3df edges[8];
	box.getEdges(edges);

	// TODO: optimize into one big drawIndexPrimitive call.

	draw3DLine(edges[5], edges[1], color);
	draw3DLine(edges[1], edges[3], color);
	draw3DLine(edges[3], edges[7], color);
	draw3DLine(edges[7], edges[5], color);
	draw3DLine(edges[0], edges[2], color);
	draw3DLine(edges[2], edges[6], color);
	draw3DLine(edges[6], edges[4], color);
	draw3DLine(edges[4], edges[0], color);
	draw3DLine(edges[1], edges[0], color);
	draw3DLine(edges[3], edges[2], color);
	draw3DLine(edges[7], edges[6], color);
	draw3DLine(edges[5], edges[4], color);
}



//! draws an 2d image
void CNullDriver::draw2DImage(const ITexture* texture, const position2d<s32>& destPos)
{
	if (!texture)
		return;

	draw2DImage(texture,destPos, rectangle<s32>(position2d<s32>(0,0),
												dimension2di(texture->getOriginalSize())));
}



//! draws a set of 2d images, using a color and the alpha channel of the
//! texture if desired. The images are drawn beginning at pos and concatenated
//! in one line. All drawings are clipped against clipRect (if != 0).
//! The subtextures are defined by the array of sourceRects and are chosen
//! by the indices given.
void CNullDriver::draw2DImageBatch(const ITexture* texture,
				const position2d<s32>& pos,
				const array<rectangle<s32> >& sourceRects,
				const array<s32>& indices,
				s32 kerningWidth,
				const rectangle<s32>* clipRect, SColor color,
				bool useAlphaChannelOfTexture)
{
	position2d<s32> target(pos);

	for (u32 i=0; i<indices.size(); ++i)
	{
		draw2DImage(texture, target, sourceRects[indices[i]],
				clipRect, color, useAlphaChannelOfTexture);
		target.X += sourceRects[indices[i]].getWidth();
		target.X += kerningWidth;
	}
}

//! draws a set of 2d images, using a color and the alpha channel of the
//! texture if desired.
void CNullDriver::draw2DImageBatch(const ITexture* texture,
				const array<position2d<s32> >& positions,
				const array<rectangle<s32> >& sourceRects,
				const rectangle<s32>* clipRect,
				SColor color,
				bool useAlphaChannelOfTexture)
{
	const u32 drawCount = min_<u32>(positions.size(), sourceRects.size());

	for (u32 i=0; i<drawCount; ++i)
	{
		draw2DImage(texture, positions[i], sourceRects[i],
				clipRect, color, useAlphaChannelOfTexture);
	}
}


//! Draws a part of the texture into the rectangle.
void CNullDriver::draw2DImage(const ITexture* texture, const rectangle<s32>& destRect,
	const rectangle<s32>& sourceRect, const rectangle<s32>* clipRect,
	const SColor* const colors, bool useAlphaChannelOfTexture)
{
	if (destRect.isValid())
		draw2DImage(texture, position2d<s32>(destRect.UpperLeftCorner),
				sourceRect, clipRect, colors?colors[0]:SColor(0xffffffff),
				useAlphaChannelOfTexture);
}


//! Draws a 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
void CNullDriver::draw2DImage(const ITexture* texture, const position2d<s32>& destPos,
				const rectangle<s32>& sourceRect,
				const rectangle<s32>* clipRect, SColor color,
				bool useAlphaChannelOfTexture)
{
}


//! Draws the outline of a 2d rectangle
void CNullDriver::draw2DRectangleOutline(const rectanglei& pos, SColor color)
{
	draw2DLine(pos.UpperLeftCorner, position2di(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y), color);
	draw2DLine(position2di(pos.LowerRightCorner.X, pos.UpperLeftCorner.Y), pos.LowerRightCorner, color);
	draw2DLine(pos.LowerRightCorner, position2di(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y), color);
	draw2DLine(position2di(pos.UpperLeftCorner.X, pos.LowerRightCorner.Y), pos.UpperLeftCorner, color);
}


//! Draw a 2d rectangle
void CNullDriver::draw2DRectangle(SColor color, const rectangle<s32>& pos, const rectangle<s32>* clip)
{
	draw2DRectangle(pos, color, color, color, color, clip);
}



//! Draws a 2d rectangle with a gradient.
void CNullDriver::draw2DRectangle(const rectangle<s32>& pos,
	SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
	const rectangle<s32>* clip)
{
}



//! Draws a 2d line.
void CNullDriver::draw2DLine(const position2d<s32>& start,
				const position2d<s32>& end, SColor color)
{
}

//! Draws a pixel
void CNullDriver::drawPixel(u32 x, u32 y, const SColor & color)
{
}


//! Draws a non filled concyclic regular 2d polyon.
void CNullDriver::draw2DPolygon(position2d<s32> center,
	f32 radius, SColor color, s32 count)
{
	if (count < 2)
		return;

	position2d<s32> first;
	position2d<s32> a,b;

	for (s32 j=0; j<count; ++j)
	{
		b = a;

		f32 p = j / (f32)count * (PI*2);
		a = center + position2d<s32>((s32)(sin(p)*radius), (s32)(cos(p)*radius));

		if (j==0)
			first = a;
		else
			draw2DLine(a, b, color);
	}

	draw2DLine(a, first, color);
}


//! returns color format
ECOLOR_FORMAT CNullDriver::getColorFormat() const
{
	return ECF_R5G6B5;
}


//! returns screen size
const dimension2d<u32>& CNullDriver::getScreenSize() const
{
	return ScreenSize;
}


//! returns the current render target size,
//! or the screen size if render targets are not implemented
const dimension2d<u32>& CNullDriver::getCurrentRenderTargetSize() const
{
	return ScreenSize;
}


// returns current frames per second value
s32 CNullDriver::getFPS() const
{
	return FPSCounter.getFPS();
}



//! returns amount of primitives (mostly triangles) were drawn in the last frame.
//! very useful method for statistics.
u32 CNullDriver::getPrimitiveCountDrawn( u32 param ) const
{
	return (0 == param) ? FPSCounter.getPrimitive() : (1 == param) ? FPSCounter.getPrimitiveAverage() : FPSCounter.getPrimitiveTotal();
}



//! Sets the dynamic ambient light color. The default color is
//! (0,0,0,0) which means it is dark.
//! \param color: New color of the ambient light.
void CNullDriver::setAmbientLight(const SColorf& color)
{
}



//! \return Returns the name of the video driver. Example: In case of the DIRECT3D8
//! driver, it would return "Direct3D8".

const wchar_t* CNullDriver::getName() const
{
	return L"Dream NullDevice";
}



//! Draws a shadow volume into the stencil buffer. To draw a stencil shadow, do
//! this: Frist, draw all geometry. Then use this method, to draw the shadow
//! volume. Then, use IVideoDriver::drawStencilShadow() to visualize the shadow.
void CNullDriver::drawStencilShadowVolume(const array<vector3df>& triangles, bool zfail, u32 debugDataVisible)
{
}


//! Fills the stencil shadow with color. After the shadow volume has been drawn
//! into the stencil buffer using IVideoDriver::drawStencilShadowVolume(), use this
//! to draw the color of the shadow.
void CNullDriver::drawStencilShadow(bool clearStencilBuffer,
		SColor leftUpEdge, SColor rightUpEdge,
		SColor leftDownEdge, SColor rightDownEdge)
{
}


//! deletes all dynamic lights there are
void CNullDriver::deleteAllDynamicLights()
{
	Lights.set_used(0);
}


//! adds a dynamic light
s32 CNullDriver::addDynamicLight(const SLight& light)
{
	Lights.push_back(light);
	return Lights.size() - 1;
}

//! Turns a dynamic light on or off
//! \param lightIndex: the index returned by addDynamicLight
//! \param turnOn: true to turn the light on, false to turn it off
void CNullDriver::turnLightOn(s32 lightIndex, bool turnOn)
{
	// Do nothing
}


//! returns the maximal amount of dynamic lights the device can handle
u32 CNullDriver::getMaximalDynamicLightAmount() const
{
	return 0;
}


//! Returns current amount of dynamic lights set
//! \return Current amount of dynamic lights set
u32 CNullDriver::getDynamicLightCount() const
{
	return Lights.size();
}


//! Returns light data which was previously set by IVideoDriver::addDynamicLight().
//! \param idx: Zero based index of the light. Must be greater than 0 and smaller
//! than IVideoDriver()::getDynamicLightCount.
//! \return Light data.
const SLight& CNullDriver::getDynamicLight(u32 idx) const
{
	if ( idx < Lights.size() )
		return Lights[idx];
	else
		return *((SLight*)0);
}


//! Creates a boolean alpha channel of the texture based of an color key.
void CNullDriver::makeColorKeyTexture(ITexture* texture,
									SColor color,
									bool zeroTexels) const
{
	if (!texture)
		return;

	if (texture->getColorFormat() != ECF_A1R5G5B5 &&
		texture->getColorFormat() != ECF_A8R8G8B8 )
	{
		Printer::log("Error: Unsupported texture color format for making color key channel.", ELL_ERROR);
		return;
	}

	if (texture->getColorFormat() == ECF_A1R5G5B5)
	{
		u16 *p = (u16*)texture->lock();

		if (!p)
		{
			Printer::log("Could not lock texture for making color key channel.", ELL_ERROR);
			return;
		}

		const dimension2d<u32> dim = texture->getSize();
		const u32 pitch = texture->getPitch() / 2;

		// color with alpha disabled (i.e. fully transparent)
		const u16 refZeroAlpha = (0x7fff & color.toA1R5G5B5());

		const u32 pixels = pitch * dim.Height;

		for (u32 pixel = 0; pixel < pixels; ++ pixel)
		{
			// If the color matches the reference color, ignoring alphas,
			// set the alpha to zero.
			if(((*p) & 0x7fff) == refZeroAlpha)
			{
				if(zeroTexels)
					(*p) = 0;
				else
					(*p) = refZeroAlpha;
			}

			++p;
		}

		texture->unlock();
	}
	else
	{
		u32 *p = (u32*)texture->lock();

		if (!p)
		{
			Printer::log("Could not lock texture for making color key channel.", ELL_ERROR);
			return;
		}

		dimension2d<u32> dim = texture->getSize();
		u32 pitch = texture->getPitch() / 4;

		// color with alpha disabled (fully transparent)
		const u32 refZeroAlpha = 0x00ffffff & color.color;

		const u32 pixels = pitch * dim.Height;
		for (u32 pixel = 0; pixel < pixels; ++ pixel)
		{
			// If the color matches the reference color, ignoring alphas,
			// set the alpha to zero.
			if(((*p) & 0x00ffffff) == refZeroAlpha)
			{
				if(zeroTexels)
					(*p) = 0;
				else
					(*p) = refZeroAlpha;
			}

			++p;
		}

		texture->unlock();
	}
	texture->regenerateMipMapLevels();
}



//! Creates an boolean alpha channel of the texture based of an color key position.
void CNullDriver::makeColorKeyTexture(ITexture* texture,
					position2d<s32> colorKeyPixelPos,
					bool zeroTexels) const
{
	if (!texture)
		return;

	if (texture->getColorFormat() != ECF_A1R5G5B5 &&
		texture->getColorFormat() != ECF_A8R8G8B8 )
	{
		Printer::log("Error: Unsupported texture color format for making color key channel.", ELL_ERROR);
		return;
	}

	SColor colorKey;

	if (texture->getColorFormat() == ECF_A1R5G5B5)
	{
		u16 *p = (u16*)texture->lock(ETLM_READ_ONLY);

		if (!p)
		{
			Printer::log("Could not lock texture for making color key channel.", ELL_ERROR);
			return;
		}

		u32 pitch = texture->getPitch() / 2;

		const u16 key16Bit = 0x7fff & p[colorKeyPixelPos.Y*pitch + colorKeyPixelPos.X];

		colorKey = A1R5G5B5toA8R8G8B8(key16Bit);
	}
	else
	{
		u32 *p = (u32*)texture->lock(ETLM_READ_ONLY);

		if (!p)
		{
			Printer::log("Could not lock texture for making color key channel.", ELL_ERROR);
			return;
		}

		u32 pitch = texture->getPitch() / 4;
		colorKey = 0x00ffffff & p[colorKeyPixelPos.Y*pitch + colorKeyPixelPos.X];
	}

	texture->unlock();
	makeColorKeyTexture(texture, colorKey, zeroTexels);
}



//! Creates a normal map from a height map texture.
//! \param amplitude: Constant value by which the height information is multiplied.
void CNullDriver::makeNormalMapTexture(ITexture* texture, f32 amplitude) const
{
	if (!texture)
		return;

	if (texture->getColorFormat() != ECF_A1R5G5B5 &&
		texture->getColorFormat() != ECF_A8R8G8B8 )
	{
		Printer::log("Error: Unsupported texture color format for making normal map.", ELL_ERROR);
		return;
	}

	dimension2d<u32> dim = texture->getSize();
	amplitude = amplitude / 255.0f;
	f32 vh = dim.Height / (f32)dim.Width;
	f32 hh = dim.Width / (f32)dim.Height;

	if (texture->getColorFormat() == ECF_A8R8G8B8)
	{
		// ECF_A8R8G8B8 version

		s32 *p = (s32*)texture->lock();

		if (!p)
		{
			Printer::log("Could not lock texture for making normal map.", ELL_ERROR);
			return;
		}

		// copy texture

		u32 pitch = texture->getPitch() / 4;

		s32* in = new s32[dim.Height * pitch];
		memcpy(in, p, dim.Height * pitch * 4);

		for (s32 x=0; x < s32(pitch); ++x)
			for (s32 y=0; y < s32(dim.Height); ++y)
			{
				// TODO: this could be optimized really a lot

				vector3df h1((x-1)*hh, nml32(x-1, y, pitch, dim.Height, in)*amplitude, y*vh);
				vector3df h2((x+1)*hh, nml32(x+1, y, pitch, dim.Height, in)*amplitude, y*vh);
				//vector3df v1(x*hh, nml32(x, y-1, pitch, dim.Height, in)*amplitude, (y-1)*vh);
				//vector3df v2(x*hh, nml32(x, y+1, pitch, dim.Height, in)*amplitude, (y+1)*vh);
				vector3df v1(x*hh, nml32(x, y+1, pitch, dim.Height, in)*amplitude, (y-1)*vh);
				vector3df v2(x*hh, nml32(x, y-1, pitch, dim.Height, in)*amplitude, (y+1)*vh);

				vector3df v = v1-v2;
				vector3df h = h1-h2;

				vector3df n = v.crossProduct(h);
				n.normalize();
				n *= 0.5f;
				n += vector3df(0.5f,0.5f,0.5f); // now between 0 and 1
				n *= 255.0f;

				s32 height = (s32)nml32(x, y, pitch, dim.Height, in);
				p[y*pitch + x] = SColor(
					height, // store height in alpha
					(s32)n.X, (s32)n.Z, (s32)n.Y).color;
			}

		delete [] in;
		texture->unlock();
	}
	else
	{
		// ECF_A1R5G5B5 version

		s16 *p = (s16*)texture->lock();

		if (!p)
		{
			Printer::log("Could not lock texture for making normal map.", ELL_ERROR);
			return;
		}

		u32 pitch = texture->getPitch() / 2;

		// copy texture

		s16* in = new s16[dim.Height * pitch];
		memcpy(in, p, dim.Height * pitch * 2);

		for (s32 x=0; x < s32(pitch); ++x)
			for (s32 y=0; y < s32(dim.Height); ++y)
			{
				// TODO: this could be optimized really a lot

				vector3df h1((x-1)*hh, nml16(x-1, y, pitch, dim.Height, in)*amplitude, y*vh);
				vector3df h2((x+1)*hh, nml16(x+1, y, pitch, dim.Height, in)*amplitude, y*vh);
				vector3df v1(x*hh, nml16(x, y-1, pitch, dim.Height, in)*amplitude, (y-1)*vh);
				vector3df v2(x*hh, nml16(x, y+1, pitch, dim.Height, in)*amplitude, (y+1)*vh);

				vector3df v = v1-v2;
				vector3df h = h1-h2;

				vector3df n = v.crossProduct(h);
				n.normalize();
				n *= 0.5f;
				n += vector3df(0.5f,0.5f,0.5f); // now between 0 and 1
				n *= 255.0f;

				p[y*pitch + x] = RGBA16((u32)n.X, (u32)n.Z, (u32)n.Y);
			}

		delete [] in;
		texture->unlock();
	}

	texture->regenerateMipMapLevels();
}


//! Returns the maximum amount of primitives (mostly vertices) which
//! the device is able to render with one drawIndexedTriangleList
//! call.
u32 CNullDriver::getMaximalPrimitiveCount() const
{
	return 0xFFFFFFFF;
}


//! checks triangle count and print warning if wrong
bool CNullDriver::checkPrimitiveCount(u32 prmCount) const
{
	const u32 m = getMaximalPrimitiveCount();

	if (prmCount > m)
	{
		char tmp[1024];
		sprintf(tmp,"Could not draw triangles, too many primitives(%u), maxium is %u.", prmCount, m);
		Printer::log(tmp, ELL_ERROR);
		return false;
	}

	return true;
}

//! Enables or disables a texture creation flag.
void CNullDriver::setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled)
{
	if (enabled && ((flag == ETCF_ALWAYS_16_BIT) || (flag == ETCF_ALWAYS_32_BIT)
		|| (flag == ETCF_OPTIMIZED_FOR_QUALITY) || (flag == ETCF_OPTIMIZED_FOR_SPEED)))
	{
		// disable other formats
		setTextureCreationFlag(ETCF_ALWAYS_16_BIT, false);
		setTextureCreationFlag(ETCF_ALWAYS_32_BIT, false);
		setTextureCreationFlag(ETCF_OPTIMIZED_FOR_QUALITY, false);
		setTextureCreationFlag(ETCF_OPTIMIZED_FOR_SPEED, false);
	}

	// set flag
	TextureCreationFlags = (TextureCreationFlags & (~flag)) |
		((((u32)!enabled)-1) & flag);
}


//! Returns if a texture creation flag is enabled or disabled.
bool CNullDriver::getTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag) const
{
	return (TextureCreationFlags & flag)!=0;
}


//! Creates a software image from a file.
IImage* CNullDriver::createImageFromFile(const path& filename)
{
	if (!filename.size())
		return 0;

	IImage* image = 0;
	IReadFile* file = FileSystem->createAndOpenFile(filename);

	if (file)
	{
		image = createImageFromFile(file);
		file->releaseRef();
	}
	else
		Printer::log("Could not open file of image", filename, ELL_WARNING);

	return image;
}


//! Creates a software image from a file.
IImage* CNullDriver::createImageFromFile(IReadFile* file)
{
	if (!file)
		return 0;

	IImage* image = 0;

	s32 i;

	// try to load file based on file extension
	for (i=SurfaceLoader.size()-1; i>=0; --i)
	{
		if (SurfaceLoader[i]->isALoadableFileExtension(file->getFileName()))
		{
			// reset file position which might have changed due to previous loadImage calls
			file->seek(0);
			image = SurfaceLoader[i]->loadImage(file);
			if (image)
				return image;
		}
	}

	// try to load file based on what is in it
	for (i=SurfaceLoader.size()-1; i>=0; --i)
	{
		// dito
		file->seek(0);
		if (SurfaceLoader[i]->isALoadableFileFormat(file))
		{
			file->seek(0);
			image = SurfaceLoader[i]->loadImage(file);
			if (image)
				return image;
		}
	}

	return 0; // failed to load
}


//! Writes the provided image to disk file
bool CNullDriver::writeImageToFile(IImage* image, const path& filename,u32 param)
{
	IWriteFile* file = FileSystem->createAndWriteFile(filename);
	if(!file)
		return false;

	bool result = writeImageToFile(image, file, param);
	file->releaseRef();

	return result;
}

//! Writes the provided image to a file.
bool CNullDriver::writeImageToFile(IImage* image, IWriteFile * file, u32 param)
{
	if(!file)
		return false;

	for (s32 i=SurfaceWriter.size()-1; i>=0; --i)
	{
		if (SurfaceWriter[i]->isAWriteableFileExtension(file->getFileName()))
		{
			bool written = SurfaceWriter[i]->writeImage(file, image, param);
			if (written)
				return true;
		}
	}
	return false; // failed to write
}


//! Creates a software image from a byte array.
IImage* CNullDriver::createImageFromData(ECOLOR_FORMAT format,
					const dimension2d<u32>& size,
					void *data, bool ownForeignMemory,
					bool deleteMemory)
{
	if(IImage::isRenderTargetOnlyFormat(format))
	{
		Printer::log("Could not create IImage, format only supported for render target textures.", ELL_WARNING);
		return 0;
	}

	return new CImage(format, size, data, ownForeignMemory, deleteMemory);
}


//! Creates an empty software image.
IImage* CNullDriver::createImage(ECOLOR_FORMAT format, const dimension2d<u32>& size)
{
	if(IImage::isRenderTargetOnlyFormat(format))
	{
		Printer::log("Could not create IImage, format only supported for render target textures.", ELL_WARNING);
		return 0;
	}

	return new CImage(format, size);
}


//! Creates a software image from another image.
IImage* CNullDriver::createImage(ECOLOR_FORMAT format, IImage *imageToCopy)
{
	Printer::log("Deprecated method, please create an empty image instead and use copyTo().", ELL_WARNING);
	if(IImage::isRenderTargetOnlyFormat(format))
	{
		Printer::log("Could not create IImage, format only supported for render target textures.", ELL_WARNING);
		return 0;
	}

	CImage* tmp = new CImage(format, imageToCopy->getDimension());
	imageToCopy->copyTo(tmp);
	return tmp;
}


//! Creates a software image from part of another image.
IImage* CNullDriver::createImage(IImage* imageToCopy, const position2d<s32>& pos, const dimension2d<u32>& size)
{
	Printer::log("Deprecated method, please create an empty image instead and use copyTo().", ELL_WARNING);
	CImage* tmp = new CImage(imageToCopy->getColorFormat(), imageToCopy->getDimension());
	imageToCopy->copyTo(tmp, position2di(0,0), rectanglei(pos,size));
	return tmp;
}


//! Creates a software image from part of a texture.
IImage* CNullDriver::createImage(ITexture* texture, const position2d<s32>& pos, const dimension2d<u32>& size)
{
	if ((pos==position2di(0,0)) && (size == texture->getSize()))
	{
		IImage* image = new CImage(texture->getColorFormat(), size, texture->lock(ETLM_READ_ONLY), false);
		texture->unlock();
		return image;
	}
	else
	{
		// make sure to avoid buffer overruns
		// make the vector a separate variable for g++ 3.x
		const vector2d<u32> leftUpper(clamp(static_cast<u32>(pos.X), 0u, texture->getSize().Width),
					clamp(static_cast<u32>(pos.Y), 0u, texture->getSize().Height));
		const rectangle<u32> clamped(leftUpper,
					dimension2du(clamp(static_cast<u32>(size.Width), 0u, texture->getSize().Width),
					clamp(static_cast<u32>(size.Height), 0u, texture->getSize().Height)));
		if (!clamped.isValid())
			return 0;
		u8* src = static_cast<u8*>(texture->lock(ETLM_READ_ONLY));
		if (!src)
			return 0;
		IImage* image = new CImage(texture->getColorFormat(), clamped.getSize());
		u8* dst = static_cast<u8*>(image->lock());
		src += clamped.UpperLeftCorner.Y * texture->getPitch() + image->getBytesPerPixel() * clamped.UpperLeftCorner.X;
		for (u32 i=0; i<clamped.getHeight(); ++i)
		{
			CColorConverter::convert_viaFormat(src, texture->getColorFormat(), clamped.getWidth(), dst, image->getColorFormat());
			src += texture->getPitch();
			dst += image->getPitch();
		}
		image->unlock();
		texture->unlock();
		return image;
	}
}


//! Sets the fog mode.
void CNullDriver::setFog(SColor color, E_FOG_TYPE fogType, f32 start, f32 end,
		f32 density, bool pixelFog, bool rangeFog)
{
	FogColor = color;
	FogType = fogType;
	FogStart = start;
	FogEnd = end;
	FogDensity = density;
	PixelFog = pixelFog;
	RangeFog = rangeFog;
}

//! Gets the fog mode.
void CNullDriver::getFog(SColor& color, E_FOG_TYPE& fogType, f32& start, f32& end,
		f32& density, bool& pixelFog, bool& rangeFog)
{
	color = FogColor;
	fogType = FogType;
	start = FogStart;
	end = FogEnd;
	density = FogDensity;
	pixelFog = PixelFog;
	rangeFog = RangeFog;
}

//! Only used by the internal engine. Used to notify the driver that
//! the window was resized.
void CNullDriver::OnResize(const dimension2d<u32>& size)
{
	if (ViewPort.getWidth() == (s32)ScreenSize.Width &&
		ViewPort.getHeight() == (s32)ScreenSize.Height)
		ViewPort = rectangle<s32>(position2d<s32>(0,0),
									dimension2di(size));

	ScreenSize = size;
}


// adds a material renderer and drops it afterwards. To be used for internal creation
s32 CNullDriver::addAndDropMaterialRenderer(IMaterialRenderer* m)
{
	s32 i = addMaterialRenderer(m);

	if (m)
		m->releaseRef();

	return i;
}

//! Adds a new material renderer to the video device.
s32 CNullDriver::addMaterialRenderer(IMaterialRenderer* renderer, const char* name)
{
	if (!renderer)
		return -1;

	SMaterialRenderer r;
	r.Renderer = renderer;
	r.Name = name;

	if (name == 0 && (MaterialRenderers.size() < (sizeof(sBuiltInMaterialTypeNames) / sizeof(char*))-1 ))
	{
		// set name of built in renderer so that we don't have to implement name
		// setting in all available renderers.
		r.Name = sBuiltInMaterialTypeNames[MaterialRenderers.size()];
	}

	MaterialRenderers.push_back(r);
	renderer->addRef();

	return MaterialRenderers.size()-1;
}


//! Sets the name of a material renderer.
void CNullDriver::setMaterialRendererName(s32 idx, const char* name)
{
	if (idx < s32(sizeof(sBuiltInMaterialTypeNames) / sizeof(char*))-1 ||
		idx >= (s32)MaterialRenderers.size())
		return;

	MaterialRenderers[idx].Name = name;
}


//! Creates material attributes list from a material, usable for serialization and more.
IAttributes* CNullDriver::createAttributesFromMaterial(const SMaterial& material,
	SAttributeReadWriteOptions* options)
{
	Printer::log("we can not being here!!!",ELL_ERROR);
	return NULL;
}


//! Fills an SMaterial structure from attributes.
void CNullDriver::fillMaterialStructureFromAttributes(SMaterial& outMaterial, IAttributes* attr)
{
	Printer::log("we can not being here!!!",ELL_ERROR);
}


//! Returns driver and operating system specific data about the IVideoDriver.
const SExposedVideoData& CNullDriver::getExposedVideoData()
{
	return ExposedData;
}


//! Returns type of video driver
E_DRIVER_TYPE CNullDriver::getDriverType() const
{
	return EDT_NULL;
}


//! deletes all material renderers
void CNullDriver::deleteMaterialRenders()
{
	// delete material renderers
	for (u32 i=0; i<MaterialRenderers.size(); ++i)
		if (MaterialRenderers[i].Renderer)
			MaterialRenderers[i].Renderer->releaseRef();

	MaterialRenderers.clear();
}


//! Returns pointer to material renderer or null
IMaterialRenderer* CNullDriver::getMaterialRenderer(u32 idx)
{
	if ( idx < MaterialRenderers.size() )
		return MaterialRenderers[idx].Renderer;
	else
		return 0;
}


//! Returns amount of currently available material renderers.
u32 CNullDriver::getMaterialRendererCount() const
{
	return MaterialRenderers.size();
}


//! Returns name of the material renderer
const char* CNullDriver::getMaterialRendererName(u32 idx) const
{
	if ( idx < MaterialRenderers.size() )
		return MaterialRenderers[idx].Name.c_str();

	return 0;
}


//! Returns pointer to the IGPUProgrammingServices interface.
IGPUProgrammingServices* CNullDriver::getGPUProgrammingServices()
{
	return this;
}


//! Adds a new material renderer to the VideoDriver, based on a high level shading language.
s32 CNullDriver::addHighLevelShaderMaterial(
	const c8* vertexShaderProgram,
	const c8* vertexShaderEntryPointName,
	E_VERTEX_SHADER_TYPE vsCompileTarget,
	const c8* pixelShaderProgram,
	const c8* pixelShaderEntryPointName,
	E_PIXEL_SHADER_TYPE psCompileTarget,
	const c8* geometryShaderProgram,
	const c8* geometryShaderEntryPointName,
	E_GEOMETRY_SHADER_TYPE gsCompileTarget,
	E_PRIMITIVE_TYPE inType, E_PRIMITIVE_TYPE outType,
	u32 verticesOut,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData, E_GPU_SHADING_LANGUAGE shadingLang)
{
	Printer::log("High level shader materials not available (yet) in this driver, sorry");
	return -1;
}


//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
//! but tries to load the programs from files.
s32 CNullDriver::addHighLevelShaderMaterialFromFiles(
		const path& vertexShaderProgramFileName,
		const c8* vertexShaderEntryPointName,
		E_VERTEX_SHADER_TYPE vsCompileTarget,
		const path& pixelShaderProgramFileName,
		const c8* pixelShaderEntryPointName,
		E_PIXEL_SHADER_TYPE psCompileTarget,
		const path& geometryShaderProgramFileName,
		const c8* geometryShaderEntryPointName,
		E_GEOMETRY_SHADER_TYPE gsCompileTarget,
		E_PRIMITIVE_TYPE inType, E_PRIMITIVE_TYPE outType,
		u32 verticesOut,
		IShaderConstantSetCallBack* callback,
		E_MATERIAL_TYPE baseMaterial,
		s32 userData, E_GPU_SHADING_LANGUAGE shadingLang)
{
	IReadFile* vsfile = 0;
	IReadFile* psfile = 0;
	IReadFile* gsfile = 0;

	if (vertexShaderProgramFileName.size() )
	{
		vsfile = FileSystem->createAndOpenFile(vertexShaderProgramFileName);
		if (!vsfile)
		{
			Printer::log("Could not open vertex shader program file",
				vertexShaderProgramFileName, ELL_WARNING);
		}
	}

	if (pixelShaderProgramFileName.size() )
	{
		psfile = FileSystem->createAndOpenFile(pixelShaderProgramFileName);
		if (!psfile)
		{
			Printer::log("Could not open pixel shader program file",
				pixelShaderProgramFileName, ELL_WARNING);
		}
	}

	if (geometryShaderProgramFileName.size() )
	{
		gsfile = FileSystem->createAndOpenFile(geometryShaderProgramFileName);
		if (!gsfile)
		{
			Printer::log("Could not open geometry shader program file",
				geometryShaderProgramFileName, ELL_WARNING);
		}
	}

	s32 result = addHighLevelShaderMaterialFromFiles(
		vsfile, vertexShaderEntryPointName, vsCompileTarget,
		psfile, pixelShaderEntryPointName, psCompileTarget,
		gsfile, geometryShaderEntryPointName, gsCompileTarget,
		inType, outType, verticesOut,
		callback, baseMaterial, userData, shadingLang);

	if (psfile)
		psfile->releaseRef();

	if (vsfile)
		vsfile->releaseRef();

	if (gsfile)
		gsfile->releaseRef();

	return result;
}


//! Like IGPUProgrammingServices::addShaderMaterial() (look there for a detailed description),
//! but tries to load the programs from files.
s32 CNullDriver::addHighLevelShaderMaterialFromFiles(
		IReadFile* vertexShaderProgram,
		const c8* vertexShaderEntryPointName,
		E_VERTEX_SHADER_TYPE vsCompileTarget,
		IReadFile* pixelShaderProgram,
		const c8* pixelShaderEntryPointName,
		E_PIXEL_SHADER_TYPE psCompileTarget,
		IReadFile* geometryShaderProgram,
		const c8* geometryShaderEntryPointName,
		E_GEOMETRY_SHADER_TYPE gsCompileTarget,
		E_PRIMITIVE_TYPE inType, E_PRIMITIVE_TYPE outType,
		u32 verticesOut,
		IShaderConstantSetCallBack* callback,
		E_MATERIAL_TYPE baseMaterial,
		s32 userData, E_GPU_SHADING_LANGUAGE shadingLang)
{
	c8* vs = 0;
	c8* ps = 0;
	c8* gs = 0;

	if (vertexShaderProgram)
	{
		const long size = vertexShaderProgram->getSize();
		if (size)
		{
			vs = new c8[size+1];
			vertexShaderProgram->read(vs, size);
			vs[size] = 0;
		}
	}

	if (pixelShaderProgram)
	{
		const long size = pixelShaderProgram->getSize();
		if (size)
		{
			// if both handles are the same we must reset the file
			if (pixelShaderProgram==vertexShaderProgram)
				pixelShaderProgram->seek(0);
			ps = new c8[size+1];
			pixelShaderProgram->read(ps, size);
			ps[size] = 0;
		}
	}

	if (geometryShaderProgram)
	{
		const long size = geometryShaderProgram->getSize();
		if (size)
		{
			// if both handles are the same we must reset the file
			if ((geometryShaderProgram==vertexShaderProgram) ||
					(geometryShaderProgram==pixelShaderProgram))
				geometryShaderProgram->seek(0);
			gs = new c8[size+1];
			geometryShaderProgram->read(gs, size);
			gs[size] = 0;
		}
	}

	s32 result = this->addHighLevelShaderMaterial(
		vs, vertexShaderEntryPointName, vsCompileTarget,
		ps, pixelShaderEntryPointName, psCompileTarget,
		gs, geometryShaderEntryPointName, gsCompileTarget,
		inType, outType, verticesOut,
		callback, baseMaterial, userData, shadingLang);

	delete [] vs;
	delete [] ps;
	delete [] gs;

	return result;
}


//! Adds a new material renderer to the VideoDriver, using pixel and/or
//! vertex shaders to render geometry.
s32 CNullDriver::addShaderMaterial(const c8* vertexShaderProgram,
	const c8* pixelShaderProgram,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData)
{
	Printer::log("Shader materials not implemented yet in this driver, sorry.");
	return -1;
}


//! Like IGPUProgrammingServices::addShaderMaterial(), but tries to load the
//! programs from files.
s32 CNullDriver::addShaderMaterialFromFiles(IReadFile* vertexShaderProgram,
	IReadFile* pixelShaderProgram,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData)
{
	c8* vs = 0;
	c8* ps = 0;

	if (vertexShaderProgram)
	{
		const long size = vertexShaderProgram->getSize();
		if (size)
		{
			vs = new c8[size+1];
			vertexShaderProgram->read(vs, size);
			vs[size] = 0;
		}
	}

	if (pixelShaderProgram)
	{
		const long size = pixelShaderProgram->getSize();
		if (size)
		{
			ps = new c8[size+1];
			pixelShaderProgram->read(ps, size);
			ps[size] = 0;
		}
	}

	s32 result = addShaderMaterial(vs, ps, callback, baseMaterial, userData);

	delete [] vs;
	delete [] ps;

	return result;
}


//! Like IGPUProgrammingServices::addShaderMaterial(), but tries to load the
//! programs from files.
s32 CNullDriver::addShaderMaterialFromFiles(const path& vertexShaderProgramFileName,
	const path& pixelShaderProgramFileName,
	IShaderConstantSetCallBack* callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData)
{
	IReadFile* vsfile = 0;
	IReadFile* psfile = 0;

	if (vertexShaderProgramFileName.size())
	{
		vsfile = FileSystem->createAndOpenFile(vertexShaderProgramFileName);
		if (!vsfile)
		{
			Printer::log("Could not open vertex shader program file",
				vertexShaderProgramFileName, ELL_WARNING);
			return -1;
		}
	}

	if (pixelShaderProgramFileName.size())
	{
		psfile = FileSystem->createAndOpenFile(pixelShaderProgramFileName);
		if (!psfile)
		{
			Printer::log("Could not open pixel shader program file",
				pixelShaderProgramFileName, ELL_WARNING);
			if (vsfile)
				vsfile->releaseRef();
			return -1;
		}
	}

	s32 result = addShaderMaterialFromFiles(vsfile, psfile, callback,
		baseMaterial, userData);

	if (psfile)
		psfile->releaseRef();

	if (vsfile)
		vsfile->releaseRef();

	return result;
}


//! Creates a render target texture.
ITexture* CNullDriver::addRenderTargetTexture(const dimension2d<u32>& size,
		const path&name, const ECOLOR_FORMAT format)
{
	return 0;
}


//! Clears the ZBuffer.
void CNullDriver::clearZBuffer()
{
}

//! Returns an image created from the last rendered frame.
IImage* CNullDriver::createScreenShot(ECOLOR_FORMAT format, E_RENDER_TARGET target)
{
	return 0;
}


// prints renderer version
void CNullDriver::printVersion()
{
	stringw namePrint = L"Using renderer: ";
	namePrint += getName();
	Printer::log(namePrint.c_str(), ELL_INFORMATION);
}


//! creates a video driver
IVideoDriver* createNullDriver(IFileSystem* io, const dimension2d<u32>& screenSize)
{
	CNullDriver* nullDriver = new CNullDriver(io, screenSize);

	// create empty material renderers
	for(u32 i=0; sBuiltInMaterialTypeNames[i]; ++i)
	{
		IMaterialRenderer* imr = new IMaterialRenderer();
		nullDriver->addMaterialRenderer(imr);
		imr->releaseRef();
	}

	return nullDriver;
}


//! Set/unset a clipping plane.
//! There are at least 6 clipping planes available for the user to set at will.
//! \param index: The plane index. Must be between 0 and MaxUserClipPlanes.
//! \param plane: The plane itself.
//! \param enable: If true, enable the clipping plane else disable it.
bool CNullDriver::setClipPlane(u32 index, const plane3df& plane, bool enable)
{
	return false;
}


//! Enable/disable a clipping plane.
void CNullDriver::enableClipPlane(u32 index, bool enable)
{
	// not necessary
}


ITexture* CNullDriver::createRenderTargetTexture(const dimension2d<u32>& size,
		const c8* name)
{
	Printer::log("createRenderTargetTexture is deprecated, use addRenderTargetTexture instead");
	ITexture* tex = addRenderTargetTexture(size, name);
	tex->addRef();
	return tex;
}

SOverrideMaterial& CNullDriver::getOverrideMaterial()
{
	return OverrideMaterial;
}


//! Get the 2d override material for altering its values
SMaterial& CNullDriver::getMaterial2D()
{
	return OverrideMaterial2D;
}


//! Enable the 2d override material
void CNullDriver::enableMaterial2D(bool enable)
{
	OverrideMaterial2DEnabled=enable;
}


dimension2du CNullDriver::getMaxTextureSize() const
{
	return dimension2du(0x10000,0x10000); // maybe large enough
}


//! Color conversion convenience function
/** Convert an image (as array of pixels) from source to destination
array, thereby converting the color format. The pixel size is
determined by the color formats.
\param sP Pointer to source
\param sF Color format of source
\param sN Number of pixels to convert, both array must be large enough
\param dP Pointer to destination
\param dF Color format of destination
*/
void CNullDriver::convertColor(const void* sP, ECOLOR_FORMAT sF, s32 sN,
		void* dP, ECOLOR_FORMAT dF) const
{
	CColorConverter::convert_viaFormat(sP, sF, sN, dP, dF);
}
