// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __E__NODE_ANIMATOR_TYPES_H_INCLUDED__
#define __E__NODE_ANIMATOR_TYPES_H_INCLUDED__
//! An enumeration for all types of built-in  node animators
enum E_NODE_ANIMATOR_TYPE
{
	//! Fly circle  node animator
	ENAT_FLY_CIRCLE = 0,

	//! Fly straight  node animator
	ENAT_FLY_STRAIGHT,

	//! Follow spline  node animator
	ENAT_FOLLOW_SPLINE,

	//! Rotation  node animator
	ENAT_ROTATION,

	//! Texture  node animator
	ENAT_TEXTURE,

	//! Deletion  node animator
	ENAT_DELETION,

	//! Collision respose  node animator
	ENAT_COLLISION_RESPONSE,

	//! FPS camera animator
	ENAT_CAMERA_FPS,

	//! Maya camera animator
	ENAT_CAMERA_MAYA,

	//! Amount of built-in  node animators
	ENAT_COUNT,

	//! Unknown  node animator
	ENAT_UNKNOWN,

	//! This enum is never used, it only forces the compiler to compile this enumeration to 32 bit.
	ENAT_FORCE_32_BIT = 0x7fffffff
};



#endif

