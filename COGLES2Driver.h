#ifndef _C_DREAM_OPENGLES2_INCLUDE__
#define _C_DREAM_OPENGLES2_INCLUDE__
#include "dreamCompileConfig.h"
#include "SDreamCreationParameters.h"
#ifndef GL_BGRA
extern int GL_BGRA;
#endif
#include "CNullDriver.h"
#include "IMaterialRendererServices.h"
#include "EDriverFeatures.h"
#include "fast_atof.h"
#include "COGLES2ExtensionHandler.h"

class COGLES2Texture;
class COGLESWrapper;
class COGLES2FixedPipelineShader;
class COGLES2Renderer2d;

class COGLES2Driver : public CNullDriver, public IMaterialRendererServices, public COGLES2ExtensionHandler
{
public:
#if defined(_DREAM_COMPILE_WITH_X11_DEVICE_)|| defined(_DREAM_WINDOWS_API)||defined(_DREAM_COMPILE_WITH_ANDROID_DEVICE_)
	COGLES2Driver( const SDreamCreationParameters& params,
				   const SExposedVideoData& data,
				   IFileSystem* io );
#endif

	//! destructor
	virtual ~COGLES2Driver();

	//! clears the zbuffer
	virtual bool beginScene( bool backBuffer = true, bool zBuffer = true,
							 SColor color = SColor( 255, 0, 0, 0 ),
							 const SExposedVideoData& videoData=SExposedVideoData(),
							 rectangle<s32>* sourceRect = 0 );

	//! presents the rendered scene on the screen, returns false if failed
	virtual bool endScene();

	//! sets transformation
	virtual void setTransform( E_TRANSFORMATION_STATE state, const matrix4& mat );

	//! draws a vertex primitive list
	virtual void drawVertexPrimitiveList( const void* vertices, u32 vertexCount,
										  const void* indexList, u32 primitiveCount,
										  E_VERTEX_TYPE vType, E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType );

	//! draws a vertex primitive list in 2d
//			virtual void draw2DVertexPrimitiveList(const void* vertices, u32 vertexCount,
//					const void* indexList, u32 primitiveCount,
//					E_VERTEX_TYPE vType=EVT_STANDARD, E_PRIMITIVE_TYPE pType=EPT_TRIANGLES, E_INDEX_TYPE iType=EIT_16BIT);

	void drawVertexPrimitiveList2d3d( const void* vertices, u32 vertexCount, const void* indexList,
			u32 primitiveCount, E_VERTEX_TYPE vType, E_PRIMITIVE_TYPE pType,
			E_INDEX_TYPE iType = EIT_16BIT, bool threed = true );

	//! queries the features of the driver, returns true if feature is available
	virtual bool queryFeature( E_VIDEO_DRIVER_FEATURE feature ) const
	{
		return FeatureEnabled[feature] && COGLES2ExtensionHandler::queryFeature(feature);
	}

	//! Sets a material.
	virtual void setMaterial( const SMaterial& material );

	//! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
	virtual void draw2DImage( const ITexture* texture, const position2d<s32>& destPos,
							  const rectangle<s32>& sourceRect, const rectangle<s32>* clipRect = 0,
							  SColor color = SColor( 255, 255, 255, 255 ), bool useAlphaChannelOfTexture = false );

	//! draws a set of 2d images
	virtual void draw2DImageBatch( const ITexture* texture,
							  const position2d<s32>& pos,
							  const array<rectangle<s32> >& sourceRects,
							  const array<s32>& indices,
							  s32 kerningWidth = 0,
							  const rectangle<s32>* clipRect = 0,
							  SColor color = SColor( 255, 255, 255, 255 ),
							  bool useAlphaChannelOfTexture = false );

	//! Draws a part of the texture into the rectangle.
	virtual void draw2DImage( const ITexture* texture, const rectangle<s32>& destRect,
							  const rectangle<s32>& sourceRect, const rectangle<s32>* clipRect = 0,
							  const SColor* const colors = 0, bool useAlphaChannelOfTexture = false );

	void draw2DImageBatch( const ITexture* texture,
						   const array<position2d<s32> >& positions,
						   const array<rectangle<s32> >& sourceRects,
						   const rectangle<s32>* clipRect,
						   SColor color,
						   bool useAlphaChannelOfTexture );

	//! draw an 2d rectangle
	virtual void draw2DRectangle( SColor color, const rectangle<s32>& pos,
								  const rectangle<s32>* clip = 0 );

	//!Draws an 2d rectangle with a gradient.
	virtual void draw2DRectangle( const rectangle<s32>& pos,
								  SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
								  const rectangle<s32>* clip = 0 );

	//! Draws a 2d line.
	virtual void draw2DLine( const position2d<s32>& start,
							 const position2d<s32>& end,
							 SColor color = SColor( 255, 255, 255, 255 ) );

	//! Draws a 3d line.
	virtual void draw3DLine( const vector3df& start,
							 const vector3df& end,
							 SColor color = SColor( 255, 255, 255, 255 ) );

	//! Draws a pixel
//			virtual void drawPixel(u32 x, u32 y, const SColor & color);

	//! Returns the name of the video driver.
	virtual const wchar_t* getName() const;

	//! deletes all dynamic lights there are
	virtual void deleteAllDynamicLights();

	//! adds a dynamic light
	virtual s32 addDynamicLight( const SLight& light );

	//! Turns a dynamic light on or off
	//! \param lightIndex: the index returned by addDynamicLight
	//! \param turnOn: true to turn the light on, false to turn it off
	virtual void turnLightOn( s32 lightIndex, bool turnOn );

	//! returns the maximal amount of dynamic lights the device can handle
	virtual u32 getMaximalDynamicLightAmount() const;

	//! Sets the dynamic ambient light color.
	virtual void setAmbientLight( const SColorf& color );

	//! return the dynamic ambient light color.
	const SColorf& getAmbientLight() const;

	//! Returns the maximum texture size supported.
	virtual dimension2du getMaxTextureSize() const;

	//! Draws a shadow volume into the stencil buffer.
	virtual void drawStencilShadowVolume( const vector3df* triangles, s32 count, bool zfail );

	//! Fills the stencil shadow with color.
	virtual void drawStencilShadow( bool clearStencilBuffer = false,
									SColor leftUpEdge = SColor( 0, 0, 0, 0 ),
									SColor rightUpEdge = SColor( 0, 0, 0, 0 ),
									SColor leftDownEdge = SColor( 0, 0, 0, 0 ),
									SColor rightDownEdge = SColor( 0, 0, 0, 0 ) );

	//! sets a viewport
	virtual void setViewPort( const rectangle<s32>& area );

	//! Only used internally by the engine
	virtual void OnResize( const dimension2d<u32>& size );

	//! Returns type of video driver
	virtual E_DRIVER_TYPE getDriverType() const;

	//! get color format of the current color buffer
	virtual ECOLOR_FORMAT getColorFormat() const;

	//! Returns the transformation set by setTransform
	virtual const matrix4& getTransform( E_TRANSFORMATION_STATE state ) const;

	//! Can be called by an IMaterialRenderer to make its work easier.
	virtual void setBasicRenderStates( const SMaterial& material, const SMaterial& lastmaterial,
									   bool resetAllRenderstates );

	//! Sets a vertex shader constant.
	virtual void setVertexShaderConstant( const f32* data, s32 startRegister, s32 constantAmount = 1 );

	//! Sets a pixel shader constant.
	virtual void setPixelShaderConstant( const f32* data, s32 startRegister, s32 constantAmount = 1 );

	//! Sets a constant for the vertex shader based on a name.
	virtual bool setVertexShaderConstant( const c8* name, const f32* floats, int count );

	//! Sets a constant for the pixel shader based on a name.
	virtual bool setPixelShaderConstant( const c8* name, const f32* floats, int count );

	//! Sets a vertex pointer the vertex shader based on a name.
	virtual bool setVertexShaderPointer( const c8* name, const void* pointer, s32 size = 3, bool normalized = false, u16 stride = 0 );

	virtual bool setVertexShaderConstant(const c8* name, const bool* bools, int count);
	virtual bool setVertexShaderConstant(const c8* name, const s32* ints, int count);
	virtual bool setPixelShaderConstant(const c8* name, const bool* bools, int count) ;
	virtual bool setPixelShaderConstant(const c8* name, const s32* ints, int count) ;
	//! sets the current Texture
	bool setTexture( u32 stage, const ITexture* texture );

	//! disables all textures beginning with fromStage.
	bool disableTextures( u32 fromStage = 0 );

	//! Adds a new material renderer to the VideoDriver
	virtual s32 addShaderMaterial( const c8* vertexShaderProgram, const c8* pixelShaderProgram,
								   IShaderConstantSetCallBack* callback, E_MATERIAL_TYPE baseMaterial, s32 userData );

	//! Adds a new material renderer to the VideoDriver
	virtual s32 addHighLevelShaderMaterial(
			const c8* vertexShaderProgram,
			const c8* vertexShaderEntryPointName = 0,
			E_VERTEX_SHADER_TYPE vsCompileTarget = EVST_VS_1_1,
			const c8* pixelShaderProgram = 0,
			const c8* pixelShaderEntryPointName = 0,
			E_PIXEL_SHADER_TYPE psCompileTarget = EPST_PS_1_1,
			const c8* geometryShaderProgram = 0,
			const c8* geometryShaderEntryPointName = "main",
			E_GEOMETRY_SHADER_TYPE gsCompileTarget = EGST_GS_4_0,
			E_PRIMITIVE_TYPE inType = EPT_TRIANGLES,
			E_PRIMITIVE_TYPE outType = EPT_TRIANGLE_STRIP,
			u32 verticesOut = 0,
			IShaderConstantSetCallBack* callback = 0,
			E_MATERIAL_TYPE baseMaterial = EMT_SOLID,
			s32 userData=0);

	//! Returns pointer to the IGPUProgrammingServices interface.
	virtual IGPUProgrammingServices* getGPUProgrammingServices();

	//! Returns a pointer to the IVideoDriver interface.
	virtual IVideoDriver* getVideoDriver();

	//! Returns the maximum amount of primitives
	virtual u32 getMaximalPrimitiveCount() const;

	virtual ITexture* addRenderTargetTexture( const dimension2d<u32>& size,
											  const path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN );
	virtual bool setRenderTarget( ITexture* texture, bool clearBackBuffer,
								  bool clearZBuffer, SColor color );

	//! set or reset special render targets
//			virtual bool setRenderTarget(E_RENDER_TARGET target, bool clearTarget,
//					bool clearZBuffer, SColor color);

	//! Sets multiple render targets
//			virtual bool setRenderTarget(const array<IRenderTarget>& texture,
//					bool clearBackBuffer=true, bool clearZBuffer=true, SColor color=SColor(0,0,0,0));

	//! Clears the ZBuffer.
	virtual void clearZBuffer();

	//! Returns an image created from the last rendered frame.
	virtual IImage* createScreenShot();

	//! checks if an OpenGL error has happend and prints it
	bool testGLError();

	//! checks if an OGLES1 error has happend and prints it
	bool testEGLError();

	//! Set/unset a clipping plane.
	virtual bool setClipPlane( u32 index, const plane3df& plane, bool enable = false );

	//! returns the current amount of user clip planes set.
	u32 getClipPlaneCount() const;

	//! returns the 0 indexed Plane
	const plane3df& getClipPlane( u32 index ) const;

	//! Enable/disable a clipping plane.
	virtual void enableClipPlane( u32 index, bool enable );

	//! Returns the graphics card vendor name.
	virtual stringc getVendorInfo()
	{
		return vendorName;
	};

	ITexture* createDepthTexture( ITexture* texture, bool shared = true );
	void removeDepthTexture( ITexture* texture );

	void renderHighLevelShader( bool state = true )
	{
		NoHighLevelShader = !state;
	}

	COGLES2FixedPipelineShader* fixedPipeline()
	{
		return FixedPipeline;
	}

	virtual void reloadShaders();

	//native ogles2 which was ogles1 extensions.

	void deleteFramebuffers( s32 n, const u32 *framebuffers );
	void deleteRenderbuffers( s32 n, const u32 *renderbuffers );

	void enableBlend();
	void disableBlend();
	void blendFunc( E_BLEND_FACTOR sFactor, E_BLEND_FACTOR dFactor );

private:

	void uploadClipPlane( u32 index );

	//! inits the opengl-es driver
	bool genericDriverInit( const dimension2d<u32>& screenSize, bool stencilBuffer );

	//! returns a device dependent texture from a software surface (IImage)
	virtual ITexture* createDeviceDependentTexture(IImage* surface, const path& name, void* mipmapData);

	//! creates a transposed matrix in supplied GLfloat array to pass to OGLES1
	inline void createGLMatrix( float gl_matrix[16], const matrix4& m );

	inline void createGLTextureMatrix( float gl_matrix[16], const matrix4& m );

	//! Map Irrlicht wrap mode to OpenGL enum
	GLint getTextureWrapMode(u8 clamp) const;

	//! Set GL pipeline to desired texture wrap modes of the material
	void setWrapMode( const SMaterial& material );

	//! sets the needed renderstates
	void setRenderStates3DMode();

	//! sets the needed renderstates
	void setRenderStates2DMode( bool alpha, bool texture, bool alphaChannel );

	// returns the current size of the screen or rendertarget
	virtual const dimension2d<u32>& getCurrentRenderTargetSize() const;

	void createMaterialRenderers();

	stringw Name;
	matrix4 Matrices[ETS_COUNT];

	//! enumeration for rendering modes such as 2d and 3d for minizing the switching of renderStates.
	enum E_RENDER_MODE
	{
		ERM_NONE = 0,   // no render state has been set yet.
		ERM_2D,     // 2d drawing rendermode
		ERM_3D      // 3d rendering mode
	};

	E_RENDER_MODE CurrentRenderMode;
	//! bool to make all renderstates reset if set to true.
	bool ResetRenderStates;
	bool Transformation3DChanged;
	u8 AntiAlias;

	SMaterial Material, LastMaterial;
	COGLES2Texture* RenderTargetTexture;
	const ITexture* CurrentTexture[MATERIAL_MAX_TEXTURES];
	array<ITexture*> DepthTextures;

	struct SUserClipPlane
	{
		plane3df Plane;
		bool Enabled;
	};

	array<SUserClipPlane> UserClipPlane;

	dimension2d<u32> CurrentRendertargetSize;

	stringc vendorName;

	matrix4 TextureFlipMatrix;

	//! Color buffer format
	ECOLOR_FORMAT ColorFormat;

	//! All the lights that have been requested; a hardware limited
	//! number of them will be used at once.
	struct RequestedLight
	{
		RequestedLight( SLight const & lightData )
				: LightData( lightData ), DesireToBeOn( true ) { }

		SLight  LightData;
		bool    DesireToBeOn;
	};

	array<RequestedLight> RequestedLights;
	SColorf AmbientLight;


#ifdef _DREAM_COMPILE_WITH_WINDOWS_DEVICE_
	HDC HDc;
#endif
#ifndef _DREAM_COMPILE_WITH_ANDROID_DEVICE_
	NativeWindowType EglWindow;
	void* EglDisplay;
	void* EglSurface;
	void* EglContext;
#endif
	COGLES2FixedPipelineShader* FixedPipeline;
	COGLES2Renderer2d* TwoDRenderer;
	bool NoHighLevelShader;

	bool BlendEnabled;
	E_BLEND_FACTOR SourceFactor;
	E_BLEND_FACTOR DestFactor;
};

#endif