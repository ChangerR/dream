// Copyright (C) 2009-2010 Amundis
// Heavily based on the OpenGL driver implemented by Nikolaus Gebhardt
// and OpenGL ES driver implemented by Christian Stehno
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h
#include "COGLES2Renderer2D.h"

#ifdef _DREAM_COMPILE_WITH_OGLES2_

const char* const COGLES2Renderer2d::sBuiltInShaderUniformNames[] =
{
	"uOrthoMatrix",
	"uUseTexture",
	"uTextureUnit",
	"uAlphaTest",
	"uAlphaValue",
	0
};
#ifdef _DREAM_COMPILE_WITH_ANDROID_DEVICE_
static const c8 vertexShaderFile[]   = "/mnt/sdcard/dream/COGLES2Renderer2D.vsh";
static const c8 fragmentShaderFile[] = "/mnt/sdcard/dream/COGLES2Renderer2D.fsh";
#else
static const char* vertexShaderFile   = "COGLES2Renderer2D.vsh";
static const char* fragmentShaderFile = "COGLES2Renderer2D.fsh";
#endif
COGLES2Renderer2d::COGLES2Renderer2d( COGLES2Driver *driver, IFileSystem *fs )
		: COGLES2SLMaterialRenderer( driver, fs, 0, 0, sBuiltInShaderUniformNames, UNIFORM_COUNT )
{

	s32 dummy = -1;
	initFromFiles( dummy, vertexShaderFile, fragmentShaderFile, false );
	useProgram();
	int texUnit = 0;
	setUniform( TEXTURE_UNIT, &texUnit );
}

void COGLES2Renderer2d::useTexture( bool param )
{
	if ( param != UseTexture )
	{
		UseTexture = param;
		int dummy = param ? 1 : 0;
		setUniform( USE_TEXTURE, &dummy );
	}
}

void COGLES2Renderer2d::useAlphaTest( bool param )
{
	if ( param != UseAlphaTest )
	{
		UseAlphaTest = param;
		int dummy = param ? 1 : 0;
		setUniform( ALPHA_TEST, &dummy );
	}
}

void COGLES2Renderer2d::setAlphaTestValue( float value )
{
	if ( value != AlphaTestValue )
	{
		AlphaTestValue = value;
		setUniform( ALPHA_VALUE, &AlphaTestValue );
	}
}

void COGLES2Renderer2d::setOrthoMatrix( const matrix4 &matrix )
{
	if ( matrix != OrthoMatrix )
	{
		OrthoMatrix = matrix;
		setUniform( ORTHO_MATRIX, OrthoMatrix.pointer() );
	}
}
 
#endif
