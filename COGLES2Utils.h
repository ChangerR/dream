#ifndef __OGLES2_UTILS_H_INCLUDED__
#define __OGLES2_UTILS_H_INCLUDED__

inline void scolorfToRGBAfloat4( const SColorf& inColor, float* outColor )
{
	outColor[0] = inColor.r;
	outColor[1] = inColor.g;
	outColor[2] = inColor.b;
	outColor[3] = inColor.a;
}

inline void vector3dfToFloat4( const core::vector3df& inVector, float* outVector, float inW )
{
	outVector[0] = inVector.X;
	outVector[1] = inVector.Y;
	outVector[2] = inVector.Z;
	outVector[3] = inW;
}

inline void vector3dfToFloat3( const core::vector3df& inVector, float* outVector )
{
	outVector[0] = inVector.X;
	outVector[1] = inVector.Y;
	outVector[2] = inVector.Z;
}

inline void scolortoFloat4( const SColor &irrColor, float *glColor )
{
	const float inv = 1.f / 255.f;
	glColor[0] = irrColor.getRed() * inv;
	glColor[1] = irrColor.getGreen() * inv;
	glColor[2] = irrColor.getBlue() * inv;
	glColor[3] = irrColor.getAlpha() * inv;
}

inline bool operator!=( const SColorf& a, const SColorf& b )
{
	if ( a.r != b.r ) return true;
	if ( a.g != b.g ) return true;
	if ( a.b != b.b ) return true;
	if ( a.a != b.a ) return true;
	return false;
}

#endif //__OGLES2_UTILS_H_INCLUDED__