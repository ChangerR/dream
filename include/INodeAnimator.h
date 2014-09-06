// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SCENE_NODE_ANIMATOR_H_INCLUDED__
#define __I_SCENE_NODE_ANIMATOR_H_INCLUDED__

#include "IReferenceCounted.h"
#include "vector3d.h"
#include "ENodeAnimatorTypes.h"

class INode;

//! Animates a scene node. Can animate position, rotation, material, and so on.
/** A scene node animator is able to animate a scene node in a very simple way. It may
change its position, rotation, scale and/or material. There are lots of animators
to choose from. You can create scene node animators with the IManager interface.
*/
class INodeAnimator
{
public:
	//! Animates a scene node.
	/** \param node Node to animate.
	\param timeMs Current time in milli seconds. */
	virtual void animateNode(INode* node, u32 timeMs) =0;

	//! Creates a clone of this animator.
	/** Please note that you will have to drop
	(IReferenceCounted::drop()) the returned pointer after calling this. */
	virtual INodeAnimator* createClone(INode* node) =0;

	//! Returns type of the scene node animator
	virtual E_NODE_ANIMATOR_TYPE getType() const
	{
		return ENAT_UNKNOWN;
	}

	//! Returns if the animator has finished.
	/** This is only valid for non-looping animators with a discrete end state.
	\return true if the animator has finished, false if it is still running. */
	virtual bool hasFinished(void) const
	{
		return false;
	}
};


#endif

