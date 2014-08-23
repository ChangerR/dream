// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#ifndef __C_OGLES2_PARALLAX_MAP_RENDERER_H_INCLUDED__
#define __C_OGLES2_PARALLAX_MAP_RENDERER_H_INCLUDED__

#include "dreamCompileConfig.h"
#ifdef _DREAM_COMPILE_WITH_OGLES2_

#include "COGLES2SLMaterialRenderer.h"
#include "IShaderConstantSetCallBack.h"


//! Class for rendering normal maps with OGLES2
class COGLES2ParallaxMapRenderer : public COGLES2SLMaterialRenderer, public IShaderConstantSetCallBack
{
public:

	//! Constructor
	COGLES2ParallaxMapRenderer( COGLES2Driver* driver, IFileSystem* fs,
								s32& outMaterialTypeNr, IMaterialRenderer* baseMaterial );

	//! Destructor
	virtual ~COGLES2ParallaxMapRenderer();

	//! Called by the engine when the vertex and/or pixel shader constants for an
	//! material renderer should be set.
	virtual void OnSetConstants( IMaterialRendererServices* services, s32 userData );

	virtual void OnSetMaterial( const SMaterial& material ) { }
	virtual void OnSetMaterial( const SMaterial& material,
								const SMaterial& lastMaterial,
								bool resetAllRenderstates, IMaterialRendererServices* services );

protected:

	bool CompiledShaders;
	f32 CurrentScale;

private:
	enum SHADER_UNIFORM
	{
		MVP_MATRIX = 0,
		LIGHT_POSITION,
		LIGHT_COLOR,
		EYE_POSITION,
		TEXTURE_UNIT0,
		TEXTURE_UNIT1,
		LIGHT_DIFFUSE,
		HEIGHT_SCALE,
		UNIFORM_COUNT
	};
	static const char* const sBuiltInShaderUniformNames[];
};


#endif
#endif

