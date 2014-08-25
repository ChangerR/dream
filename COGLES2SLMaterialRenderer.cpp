#include "dreamCompileConfig.h"
#ifdef _DREAM_COMPILE_WITH_OGLES2_

#include "COGLES2SLMaterialRenderer.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IMaterialRendererServices.h"
#include "IVideoDriver.h"
#include "os.h"
#include "COGLES2Driver.h"
#include "EVertexAttributes.h"
#include "COGLES2Texture.h"

#include <GLES2/gl2.h>

#ifdef _DREAM_COMPILE_WITH_ANDROID_DEVICE_
#include <android/log.h>
#endif
#include <stdio.h>
#include <wchar.h>

#ifdef _MSC_VER
#define snprintf _snprintf
#endif
//! Constructor
COGLES2SLMaterialRenderer::COGLES2SLMaterialRenderer( COGLES2Driver* driver,
													  s32& outMaterialTypeNr, const c8* vertexShaderProgram,
													  const c8* pixelShaderProgram,
													  IShaderConstantSetCallBack* callback,
													  IMaterialRenderer* baseMaterial,
													  s32 userData )
		: Driver( driver ), CallBack( callback ), BaseMaterial( baseMaterial ), FileSystem( 0 ),
		Program( 0 ), UserData( userData ), UniformStringTable( 0 ), UniformCount( 0 )
{
	//entry points must always be main, and the compile target isn't selectable
	//it is fine to ignore what has been asked for, as the compiler should spot anything wrong
	//just check that GLSL is available

	if ( BaseMaterial )
		BaseMaterial->addRef();

	if ( CallBack )
		CallBack->addRef();

	init( outMaterialTypeNr, vertexShaderProgram, pixelShaderProgram );
}


//! constructor only for use by derived classes who want to
//! create a fall back material for example.
COGLES2SLMaterialRenderer::COGLES2SLMaterialRenderer( COGLES2Driver* driver,
													  IFileSystem* fs,
													  IShaderConstantSetCallBack* callback,
													  IMaterialRenderer* baseMaterial,
													  const char* const * uniformStringTable,
													  const u32& uniformCount,
													  s32 userData )
		: Driver( driver ), CallBack( callback ), BaseMaterial( baseMaterial ), FileSystem( fs ),
		Program( 0 ), UserData( userData ), UniformStringTable( uniformStringTable ),
		UniformCount( uniformCount )
{
	if ( BaseMaterial )
		BaseMaterial->addRef();

	if ( CallBack )
		CallBack->addRef();
}


//! Destructor
COGLES2SLMaterialRenderer::~COGLES2SLMaterialRenderer()
{
	if ( CallBack )
		CallBack->releaseRef();

	if ( Program )
	{
		glDeleteProgram( Program );
		Program = 0;
	}

	UniformInfo.clear();

	AttributeInfo.clear();

	if ( BaseMaterial )
		BaseMaterial->releaseRef();
}

void COGLES2SLMaterialRenderer::init( s32& outMaterialTypeNr,
									  const c8* vertexShaderProgram,
									  const c8* pixelShaderProgram,
									  bool registerMaterial )
{
	outMaterialTypeNr = -1;

	if ( Program == 0 && !createProgram() )
		return;

	if ( vertexShaderProgram )
		if ( !createShader( GL_VERTEX_SHADER, vertexShaderProgram, "" ) )
			return;

	if ( pixelShaderProgram )
		if ( !createShader( GL_FRAGMENT_SHADER, pixelShaderProgram, "" ) )
			return;

	if ( !linkProgram() )
		return;

	// register myself as new material
	if ( registerMaterial )
		outMaterialTypeNr = Driver->addMaterialRenderer( this );
}

void COGLES2SLMaterialRenderer::initFromFiles( s32 &outMaterialTypeNr,
											   const c8 *vertexShaderFile,
											   const c8 *pixelShaderFile,
											   bool registerMaterial )
{
	stringc msg = "load shader: ";
	msg += vertexShaderFile;
	Printer::log( msg.c_str(), ELL_INFORMATION );

	if ( !createProgram() )
	{
		Printer::log( "Could not create shader program.", ELL_ERROR );
	}

	if ( !readVertexShader( vertexShaderFile ) )
	{
		Printer::log( "Error reading fixed pipeline vertex shader.", ELL_ERROR );
	}

	if ( !readFragmentShader( pixelShaderFile ) )
	{
		Printer::log( "Error reading fixed pipeline fragment shader.", ELL_ERROR );
	}

	for ( size_t i = 0; i < EVA_COUNT; ++i )
		glBindAttribLocation( Program, i, sBuiltInVertexAttributeNames[i] );

	if ( !linkProgram() )
	{
		Printer::log( "Error linking fixed pipeline shader program.", ELL_ERROR );
	}

	if ( registerMaterial )
		outMaterialTypeNr = Driver->addMaterialRenderer( this );
	Printer::log("Shader loaded");
}

void COGLES2SLMaterialRenderer::reload( const c8* vertexShaderProgram,
                       const c8* pixelShaderProgram) {
	GLsizei shaderCount;
	GLuint shaderHandles[2];
	glGetAttachedShaders( Program, 2, &shaderCount, shaderHandles );
	glDetachShader( Program, shaderHandles[0] );
	glDeleteShader( shaderHandles[0] );
	glDetachShader( Program, shaderHandles[1] );
	glDeleteShader( shaderHandles[1] );		   
	if ( !createShader( GL_VERTEX_SHADER, vertexShaderProgram, "" ) )
	{
		Printer::log( "Error reading fixed pipeline vertex shader.", ELL_ERROR );
	}

	if ( !createShader( GL_FRAGMENT_SHADER, vertexShaderProgram, "" ) )
	{
		Printer::log( "Error reading fixed pipeline fragment shader.", ELL_ERROR );
	}

	if ( !linkProgram() )
	{
		Printer::log( "Error linking fixed pipeline shader program.", ELL_ERROR );
	}		   
}
		
void COGLES2SLMaterialRenderer::reloadFromFiles( const c8 *vertexShaderFile,
												 const c8 *pixelShaderFile )
{
	GLsizei shaderCount;
	GLuint shaderHandles[2];
	glGetAttachedShaders( Program, 2, &shaderCount, shaderHandles );
	glDetachShader( Program, shaderHandles[0] );
	glDeleteShader( shaderHandles[0] );
	glDetachShader( Program, shaderHandles[1] );
	glDeleteShader( shaderHandles[1] );
	if ( !readVertexShader( vertexShaderFile ) )
	{
		Printer::log( "Error reading fixed pipeline vertex shader.", ELL_ERROR );
	}

	if ( !readFragmentShader( pixelShaderFile ) )
	{
		Printer::log( "Error reading fixed pipeline fragment shader.", ELL_ERROR );
	}

	if ( !linkProgram() )
	{
		Printer::log( "Error linking fixed pipeline shader program.", ELL_ERROR );
	}
}

bool COGLES2SLMaterialRenderer::readShader( GLenum shaderType, const c8* shaderFile )
{
	char buf[512];
	IReadFile* file = 0;
	file = FileSystem->createAndOpenFile( shaderFile );
	if ( !file )
	{
		snprintf( buf, 512, "Could not open shader file : %s", shaderFile );
		Printer::log( buf, ELL_ERROR );
		return false;
	}
	const long size = file->getSize();
	if ( !size )
	{
		snprintf( buf, 512, "%s shader file is empty", shaderFile );
		Printer::log( buf, ELL_ERROR );
		file->releaseRef();
		return false;
	}
	
	c8* shader = new c8[size+1];
	file->read( shader, size );
	shader[size] = 0;
	bool success = createShader( shaderType, shader, shaderFile );
	file->releaseRef();
	delete shader;
	return success;
}

bool COGLES2SLMaterialRenderer::readVertexShader( const c8 *vertexShaderFile )
{
	return readShader( GL_VERTEX_SHADER, vertexShaderFile );
}

bool COGLES2SLMaterialRenderer::readFragmentShader( const c8 *fragmentShaderFile )
{
	return readShader( GL_FRAGMENT_SHADER, fragmentShaderFile );
}

bool COGLES2SLMaterialRenderer::OnRender( IMaterialRendererServices* service,
										  E_VERTEX_TYPE vtxtype )
{
	// call callback to set shader constants and attributes
	if ( CallBack && Program )
	{
		CallBack->OnSetAttribute( this, UserData );
		CallBack->OnSetConstants( this, UserData );
	}

	return true;
}


bool COGLES2SLMaterialRenderer::PostRender( IMaterialRendererServices* service,
											E_VERTEX_TYPE vtxtype )
{
	// call callback to unset shader attributes
	if ( CallBack && Program )
		CallBack->OnUnSetAttribute( this, UserData );

	return true;
}

void COGLES2SLMaterialRenderer::OnSetMaterial( const SMaterial& material,
											   const SMaterial& lastMaterial,
											   bool resetAllRenderstates,
											   IMaterialRendererServices* services )
{
	if ( material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates )
	{
		if ( Program )
			glUseProgram( Program );

		//let callback know used material
		if ( CallBack )
			CallBack->OnSetMaterial( material );
	}
	//if (BaseMaterial)
	//BaseMaterial->OnSetMaterial(material, material, true, this);

	//for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
	//  Driver->setActiveTexture(i, material.getTexture(i));
	Driver->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
}


void COGLES2SLMaterialRenderer::OnUnsetMaterial()
{
	glUseProgram( 0 );

	//if (BaseMaterial)
	//  BaseMaterial->OnUnsetMaterial();
}

//! Returns if the material is transparent.
bool COGLES2SLMaterialRenderer::isTransparent() const
{
	return BaseMaterial ? BaseMaterial->isTransparent() : false;
}

bool COGLES2SLMaterialRenderer::createProgram()
{
	Program = glCreateProgram();
	return true;
}

bool COGLES2SLMaterialRenderer::createShader( GLenum shaderType, const char* shader, const char* shaderFile )
{
	GLuint shaderHandle = glCreateShader( shaderType );

	Printer::log("Loading shader", shaderFile);
	glShaderSource( shaderHandle, 1, &shader, NULL );
	glCompileShader( shaderHandle );

	int status = 0;

	glGetShaderiv( shaderHandle, GL_COMPILE_STATUS, &status );

	if ( !status )
	{
		char buf[512];
		snprintf( buf, 512, "GLSL shader failed to compile : %s", shaderFile );
		Printer::log( buf, ELL_ERROR );

		// check error message and log it
		int maxLength = 0;
		GLsizei length;

		glGetShaderiv( shaderHandle, GL_INFO_LOG_LENGTH, &maxLength );

		char *pInfoLog = new char[maxLength];
		glGetShaderInfoLog( shaderHandle, maxLength, &length, pInfoLog );
		Printer::log( reinterpret_cast<const c8*>( pInfoLog ), ELL_ERROR );
		delete [] pInfoLog;

		return false;
	}

	glAttachShader( Program, shaderHandle );

	return true;
}

bool COGLES2SLMaterialRenderer::linkProgram()
{
	glLinkProgram( Program );

	int status = 0;

	glGetProgramiv( Program, GL_LINK_STATUS, &status );

	if ( !status )
	{
		Printer::log( "GLSL shader program failed to link", ELL_ERROR );
		// check error message and log it
		int maxLength = 0;
		GLsizei length;

		glGetProgramiv( Program, GL_INFO_LOG_LENGTH, &maxLength );

		char *pInfoLog = new char[maxLength];
		glGetProgramInfoLog( Program, maxLength, &length, pInfoLog );
		Printer::log( reinterpret_cast<const c8*>( pInfoLog ), ELL_ERROR );
		delete [] pInfoLog;

		return false;
	}

	// get uniforms information

	int num = 0;

	glGetProgramiv( Program, GL_ACTIVE_UNIFORMS, &num );

	int maxlen = 0;

	glGetProgramiv( Program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen );

	if ( maxlen == 0 && num != 0 )
	{
		Printer::log( "GLSL: failed to retrieve uniform information", ELL_ERROR );
		return false;
	}

	c8 *buf = new c8[maxlen];

	UniformInfo.clear();
	UniformInfo.reallocate( num );

	array<stringc> names( num );
	array<SUniformInfo> uni( num );

	for ( int i = 0; i < num; ++i )
	{
		memset( buf, 0, maxlen );
		GLint size;
		SUniformInfo ui;
		glGetActiveUniform( Program, i, maxlen, 0, &size, &ui.type, reinterpret_cast<char*>( buf ) );
		ui.location = glGetUniformLocation( Program, buf );
		uni.push_back( ui );
		names.push_back( buf );
	}

	delete [] buf;

	for ( int i = 0; i < UniformCount; ++i )
	{
		int j;
		for ( j = 0; j < num; ++j )
		{
			if ( names[j] == UniformStringTable[i] )
				break;
		}
		if ( j < num )
		{
			UniformInfo.push_back( uni[j] );
		}
		else
		{
			char buf[512];
			snprintf( buf, 512, "Unable to find uniform : %s", UniformStringTable[i] );
			Printer::log( buf, ELL_WARNING );
			SUniformInfo blank;
			blank.location = -1;
			blank.type = GL_INVALID_ENUM;
			UniformInfo.push_back( blank );
		}
	}

	return true;
}


void COGLES2SLMaterialRenderer::setBasicRenderStates( const SMaterial& material,
													  const SMaterial& lastMaterial,
													  bool resetAllRenderstates )
{
	// forward
	Driver->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
}


bool COGLES2SLMaterialRenderer::setVertexShaderConstant( const c8* name, const f32* floats, int count )
{
	return setPixelShaderConstant( name, floats, count );
}

bool COGLES2SLMaterialRenderer::setVertexShaderConstant(const c8* name, const bool* bools, int count) {
	Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
	return false;
}

bool COGLES2SLMaterialRenderer::setVertexShaderConstant(const c8* name, const s32* ints, int count) {
	Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
	return false;
}

void COGLES2SLMaterialRenderer::setVertexShaderConstant( const f32* data, s32 startRegister, s32 constantAmount )
{
	Printer::log( "Cannot set constant, please use high level shader call instead.", ELL_WARNING );
}

bool COGLES2SLMaterialRenderer::setVertexShaderPointer( const c8* name, const void* pointer,
														s32 size, bool normalized, u16 stride )
{
	Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );

	return false;
}

void COGLES2SLMaterialRenderer::enableVertexShaderPointer( const c8* name )
{
	Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
}

void COGLES2SLMaterialRenderer::disableVertexShaderPointer( const c8* name )
{
	Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
}

bool COGLES2SLMaterialRenderer::setPixelShaderConstant( const c8* name, const f32* floats, int count )
{
	Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
	return false;
}

bool COGLES2SLMaterialRenderer::setPixelShaderConstant(const c8* name, const bool* bools, int count)  {
	Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
	return false;
}
bool COGLES2SLMaterialRenderer::setPixelShaderConstant(const c8* name, const s32* ints, int count) {
	Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
	return false;
}

bool COGLES2SLMaterialRenderer::setUniform( int index, const void* data, int count )
{
	SUniformInfo& ui = UniformInfo[index];
	if ( ui.location == -1 )
		return false;
	switch ( ui.type )
	{
		case GL_FLOAT:
			glUniform1fv( ui.location, count, static_cast<const GLfloat*>( data ) );
			break;
		case GL_FLOAT_VEC2:
			glUniform2fv( ui.location, count, static_cast<const GLfloat*>( data ) );
			break;
		case GL_FLOAT_VEC3:
			glUniform3fv( ui.location, count, static_cast<const GLfloat*>( data ) );
			break;
		case GL_FLOAT_VEC4:
			glUniform4fv( ui.location, count, static_cast<const GLfloat*>( data ) );
			break;
		case GL_FLOAT_MAT2:
			glUniformMatrix2fv( ui.location, count, false, static_cast<const GLfloat*>( data ) );
			break;
		case GL_FLOAT_MAT3:
			glUniformMatrix3fv( ui.location, count, false, static_cast<const GLfloat*>( data ) );
			break;
		case GL_FLOAT_MAT4:
			glUniformMatrix4fv( ui.location, count, false, static_cast<const GLfloat*>( data ) );
			break;
		default:
			glUniform1iv( ui.location, count, static_cast<const GLint*>( data ) );
			break;
	}
	return !Driver->testGLError();
}

bool COGLES2SLMaterialRenderer::setTextureUnit( const c8* name, int unit )
{
	Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
	return false;
}

bool COGLES2SLMaterialRenderer::enableMaterialTexture( const c8* name, int materialId )
{
	ITexture * t = Driver->getTextureByIndex( materialId );
	COGLES2Texture * tex = reinterpret_cast<COGLES2Texture*>( t );
	if ( !tex )
		return false;
	int unit = tex->getOGLES2TextureName();

	glActiveTexture( GL_TEXTURE0 + unit );
	glBindTexture( GL_TEXTURE_2D, unit );

	return setTextureUnit( name, unit );
}

bool COGLES2SLMaterialRenderer::disableMaterialTexture( int materialId )
{
	COGLES2Texture * tex = reinterpret_cast<COGLES2Texture*>( Driver->getTextureByIndex( materialId ) );
	if ( !tex )
		return false;

	int unit = tex->getOGLES2TextureName();

	glActiveTexture( GL_TEXTURE0 + unit );
	glBindTexture( GL_TEXTURE_2D, 0 );

	return true;
}

void COGLES2SLMaterialRenderer::setPixelShaderConstant( const f32* data, s32 startRegister, s32 constantAmount )
{
	Printer::log( "Cannot set constant, use high level shader call.", ELL_WARNING );
}

IVideoDriver* COGLES2SLMaterialRenderer::getVideoDriver()
{
	return Driver;
}

void COGLES2SLMaterialRenderer::useProgram()
{
	glUseProgram( Program );
}


#endif //_DREAM_COMPILE_WITH_OPENGLES2_