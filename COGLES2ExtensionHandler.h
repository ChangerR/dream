#ifndef __C_OGLES2_EXTENSION_HANDLER_H_INCLUDED__
#define __C_OGLES2_EXTENSION_HANDLER_H_INCLUDED__

#include "dreamCompileConfig.h"

#ifdef _DREAM_COMPILE_WITH_OGLES2_
#include "os.h"
#include "EDriverFeatures.h"

#if defined(_DREAM_COMPILE_WITH_ANDROID_DEVICE_)
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#else
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#endif

class COGLES2Driver;
class COGLES2ExtensionHandler
{
public:
	enum EOGLES2Features
	{
		DREAM_AMD_compressed_3DC_texture = 0, //39
		DREAM_AMD_compressed_ATC_texture, //40
		DREAM_AMD_performance_monitor, //50
		DREAM_AMD_program_binary_Z400, //48
		DREAM_ARB_texture_env_combine, //ogl, IMG simulator
		DREAM_ARB_texture_env_dot3, //ogl, IMG simulator
		DREAM_EXT_multi_draw_arrays, //ogl, IMG simulator
		DREAM_EXT_texture_compression_dxt1, //49
		DREAM_EXT_texture_filter_anisotropic, //41
		DREAM_EXT_texture_format_BGRA8888, //51
		DREAM_EXT_texture_type_2_10_10_10_REV, //42
		DREAM_IMG_read_format, //53
		DREAM_IMG_texture_compression_pvrtc, //54
		DREAM_IMG_texture_env_enhanced_fixed_function, // non-standard
		DREAM_IMG_texture_format_BGRA8888, // replaced by EXT version
		DREAM_IMG_user_clip_planes, // non-standard
		DREAM_IMG_vertex_program, // non-standard
		DREAM_NV_fence, //52
		DREAM_OES_blend_equation_separate, //1
		DREAM_OES_blend_func_separate, //2
		DREAM_OES_blend_subtract, //3
		DREAM_OES_byte_coordinates, //4
		DREAM_OES_compressed_ETC1_RGB8_texture, //5
		DREAM_OES_compressed_paletted_texture, //6
		DREAM_OES_depth24, //24
		DREAM_OES_depth32, //25
		DREAM_OES_depth_texture, //43
		DREAM_OES_draw_texture, //7
		DREAM_OES_EGL_image, //23
		DREAM_OES_element_index_uint, //26
		DREAM_OES_extended_matrix_palette, //8
		DREAM_OES_fbo_render_mipmap, //27
		DREAM_OES_fixed_point, //9
		DREAM_OES_fragment_precision_high, //28
		DREAM_OES_framebuffer_object, //10
		DREAM_OES_get_program_binary, //47
		DREAM_OES_mapbuffer, //29
		DREAM_OES_matrix_get, //11
		DREAM_OES_matrix_palette, //12
		DREAM_OES_packed_depth_stencil, //44
		DREAM_OES_point_size_array, //14
		DREAM_OES_point_sprite, //15
		DREAM_OES_query_matrix, //16
		DREAM_OES_read_format, //17
		DREAM_OES_rgb8_rgba8, //30
		DREAM_OES_single_precision, //18
		DREAM_OES_standard_derivatives, //45
		DREAM_OES_stencil1, //31
		DREAM_OES_stencil4, //32
		DREAM_OES_stencil8, //33
		DREAM_OES_stencil_wrap, //19
		DREAM_OES_texture_3D, //34
		DREAM_OES_texture_cube_map, //20
		DREAM_OES_texture_env_crossbar, //21
		DREAM_OES_texture_float, //36
		DREAM_OES_texture_float_linear, //35
		DREAM_OES_texture_half_float, //36
		DREAM_OES_texture_half_float_linear, //35
		DREAM_OES_texture_mirrored_repeat, //22
		DREAM_OES_texture_npot, //37
		DREAM_OES_vertex_half_float, //38
		DREAM_OES_vertex_type_10_10_10_2, //46
		DREAM_QCOM_driver_control, //55
		DREAM_QCOM_performance_monitor_global_mode, //56

		DREAM_OGLES2_Feature_Count
	};

	//! queries the features of the driver, returns true if feature is available
	bool queryOpenGLFeature( EOGLES2Features feature ) const
	{
		return FeatureAvailable[feature];
	}


protected:
	COGLES2ExtensionHandler();

	bool queryFeature( E_VIDEO_DRIVER_FEATURE feature ) const
	{
		switch ( feature )
		{
			case EVDF_RENDER_TO_TARGET:
				return true;
			case EVDF_MULTITEXTURE:
				return MultiTextureExtension;
			case EVDF_BILINEAR_FILTER:
				return true;
			case EVDF_MIP_MAP:
				return true;
			case EVDF_MIP_MAP_AUTO_UPDATE:
				return Version > 100; // Supported in version 1.1
			case EVDF_STENCIL_BUFFER:
				return StencilBuffer;
			case EVDF_TEXTURE_NSQUARE:
				return true; // non-square is always supported
			default:
				return false;
		}
	}

	void dump() const;

#ifndef _DREAM_COMPILE_WITH_ANDROID_DEVICE_
	void initExtensions( COGLES2Driver* driver,
						 EGLDisplay display,
						 bool withStencil );
#else
	void initExtensions( COGLES2Driver* driver,
						 bool withStencil );
#endif

protected:
#ifndef _DREAM_COMPILE_WITH_ANDROID_DEVICE_
	u16 EGLVersion;
#endif
	u16 Version;
	u8 MaxTextureUnits;
	u8 MaxLights;
	u8 MaxAnisotropy;
	u8 MaxUserClipPlanes;
	u32 MaxTextureSize;
	bool CommonProfile;
	bool MultiTextureExtension;
	bool MultiSamplingExtension;
	bool StencilBuffer;
	bool FeatureAvailable[DREAM_OGLES2_Feature_Count];
};


#endif // _DREAM_COMPILE_WITH_OGLES21_
#endif
