// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_SCENE_NODE_ANIMATOR_CAMERA_MAYA_H_INCLUDED__
#define __I_SCENE_NODE_ANIMATOR_CAMERA_MAYA_H_INCLUDED__

#include "ISceneNodeAnimator.h"

namespace irr
{

namespace scene
{

	//! Special scene node animator for Maya-style cameras
	/** This scene node animator can be attached to a camera to make it act like a 3d
	modelling tool.
	The camera is moving relative to the target with the mouse, by pressing either
	of the three buttons.
	*/
	class ISceneNodeAnimatorCameraMaya : public ISceneNodeAnimator
	{
	public:

		//! Returns the speed of movement
		virtual f32 getMoveSpeed() const = 0;

		//! Sets the speed of movement
		virtual void setMoveSpeed(f32 moveSpeed) = 0;

		//! Returns the rotation speed
		virtual f32 getRotateSpeed() const = 0;

		//! Set the rotation speed
		virtual void setRotateSpeed(f32 rotateSpeed) = 0;

		//! Returns the zoom speed
		virtual f32 getZoomSpeed() const = 0;

		//! Set the zoom speed
		virtual void setZoomSpeed(f32 zoomSpeed) = 0;

	};

} // end namespace scene
} // end namespace irr

#endif

