/// \file Battery.h
/// \brief Interface for the game object class CBattery.

#pragma once

#include "Object.h"

/// \brief The game object. 
///
/// CBattery is the abstract representation of an object.

class CBattery : public CObject {
	friend class CObjectManager; ///< The object manager needs access to manage objects, including the player.

private:
	BoundingSphere m_Sphere; ///< Bounding sphere.
	BoundingBox m_Aabb; ///< Axially aligned bounding box.

	//	Vector2 m_vPos = Vector2::Zero;
	Vector2 m_vOldPos = Vector2::Zero; ///< Last position.
	Vector2 m_vVelocity = Vector2::Zero; ///< Velocity.

	Vector2 m_vRadius = Vector2::Zero; ///< Half width and height of object sprite.

	int uniqueID;		//Added so gold has an ID for History

public:
	CBattery(); ///< default Constructor
	CBattery(const Vector2& p); ///< Constructor
	void Updates();
	int getIndex();
	void SetUniqueID(int newID) { uniqueID = newID; };
	int  GetUniqueID() { return uniqueID; };



}; //CBattery