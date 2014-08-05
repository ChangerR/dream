#ifndef __E_SHADER_TYPES_H_INCLUDED__
#define __E_SHADER_TYPES_H_INCLUDED__

#include "dtype.h"

//! Compile target enumeration for the addHighLevelShaderMaterial() method.
enum E_VERTEX_SHADER_TYPE
{
	EVST_VS_1_1 = 0,
	EVST_VS_2_0,
	EVST_VS_2_a,
	EVST_VS_3_0,
	EVST_VS_4_0,
	EVST_VS_4_1,
	EVST_VS_5_0,

	//! This is not a type, but a value indicating how much types there are.
	EVST_COUNT
};

//! Compile target enumeration for the addHighLevelShaderMaterial() method.
enum E_PIXEL_SHADER_TYPE
{
	EPST_PS_1_1 = 0,
	EPST_PS_1_2,
	EPST_PS_1_3,
	EPST_PS_1_4,
	EPST_PS_2_0,
	EPST_PS_2_a,
	EPST_PS_2_b,
	EPST_PS_3_0,
	EPST_PS_4_0,
	EPST_PS_4_1,
	EPST_PS_5_0,

	//! This is not a type, but a value indicating how much types there are.
	EPST_COUNT
};


//! Enum for supported geometry shader types
enum E_GEOMETRY_SHADER_TYPE
{
	EGST_GS_4_0 = 0,

	//! This is not a type, but a value indicating how much types there are.
	EGST_COUNT
};


#endif // __E_SHADER_TYPES_H_INCLUDED__

