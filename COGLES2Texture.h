#ifndef __C_OGLES2_TEXTURE_H_INCLUDED__
#define __C_OGLES2_TEXTURE_H_INCLUDED__

#include "ITexture.h"
#include "IImage.h"

#include "dreamCompileConfig.h"
#ifdef _DREAM_COMPILE_WITH_OGLES2_

class COGLES2Driver;
//! OGLES21 texture.
class COGLES2Texture : public ITexture
{
public:

	//! constructor
	COGLES2Texture( IImage* surface, const path& name, COGLES2Driver* driver = 0 );

	//! destructor
	virtual ~COGLES2Texture();

	//! lock function
	virtual void* lock( E_TEXTURE_LOCK_MODE mode=ETLM_READ_WRITE, u32 mipmapLevel=0 );

	//! unlock function
	virtual void unlock();

	//! Returns original size of the texture (image).
	virtual const dimension2d<u32>& getOriginalSize() const;

	//! Returns size of the texture.
	virtual const dimension2d<u32>& getSize() const;

	//! returns driver type of texture (=the driver, that created it)
	virtual E_DRIVER_TYPE getDriverType() const;

	//! returns color format of texture
	virtual ECOLOR_FORMAT getColorFormat() const;

	//! returns pitch of texture (in bytes)
	virtual u32 getPitch() const;

	//! return open gl texture name
	u32 getOGLES2TextureName() const;

	//! return whether this texture has mipmaps
	virtual bool hasMipMaps() const;

	//! Regenerates the mip map levels of the texture.
	virtual void regenerateMipMapLevels(void* mipmapData=0);

	//! Is it a render target?
	virtual bool isRenderTarget() const;

	//! Is it a FrameBufferObject?
	virtual bool isFrameBufferObject() const;

	//! Bind RenderTargetTexture
	void bindRTT();

	//! Unbind RenderTargetTexture
	void unbindRTT();

	//! sets whether this texture is intended to be used as a render target.
	void setIsRenderTarget( bool isTarget );

protected:

	//! protected constructor with basic setup, no GL texture name created, for derived classes
	COGLES2Texture( const path& name, COGLES2Driver* driver );

	//! get the desired color format based on texture creation flags and the input format.
	ECOLOR_FORMAT getBestColorFormat( ECOLOR_FORMAT format );

	//! convert the image into an internal image with better properties for this driver.
	void getImageData( IImage* image );

	//! copies the the texture into an open gl texture.
	void copyTexture( bool newTexture = true );

	dimension2d<u32> ImageSize;
	COGLES2Driver* Driver;
	IImage* Image;

	u32 TextureName;
	s32 InternalFormat;
	u32 PixelFormat;
	u32 PixelType;

	bool HasMipMaps;
	bool IsRenderTarget;
	bool AutomaticMipmapUpdate;
	bool UseStencil;
	bool ReadOnlyLock;
};


//! OGLES21 FBO texture.
class COGLES2FBOTexture : public COGLES2Texture
{
public:

	//! FrameBufferObject constructor
	COGLES2FBOTexture( const dimension2d<u32>& size, const path& name, COGLES2Driver* driver = 0, ECOLOR_FORMAT format = ECF_UNKNOWN );

	//! destructor
	virtual ~COGLES2FBOTexture();

	//! Is it a FrameBufferObject?
	virtual bool isFrameBufferObject() const;

	//! Bind RenderTargetTexture
	virtual void bindRTT();

	//! Unbind RenderTargetTexture
	virtual void unbindRTT();

	ITexture* DepthTexture;
protected:
	u32 ColorFrameBuffer;
};


//! OGLES21 FBO depth texture.
class COGLES2FBODepthTexture : public COGLES2FBOTexture
{
public:
	//! FrameBufferObject depth constructor
	COGLES2FBODepthTexture( const dimension2d<u32>& size, const path& name, COGLES2Driver* driver = 0, bool useStencil = false );

	//! destructor
	virtual ~COGLES2FBODepthTexture();

	//! Bind RenderTargetTexture
	virtual void bindRTT();

	//! Unbind RenderTargetTexture
	virtual void unbindRTT();

	void attach( ITexture* );

protected:
	u32 DepthRenderBuffer;
	u32 StencilRenderBuffer;
	bool UseStencil;
};

#endif
#endif // _DREAM_COMPILE_WITH_OGLES2_
