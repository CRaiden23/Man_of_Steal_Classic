/// \file Object.h
/// \brief Interface for the game object class CObject.

#pragma once

#include "GameDefines.h"
#include "Renderer.h"
#include "Common.h"
#include "Component.h"
#include "SpriteDesc.h"

/// \brief The game object. 
///
/// CObject is the abstract representation of an object.

class CObject:
  public CCommon,
  public CComponent,
  public CSpriteDesc2D
{
  friend class CObjectManager; ///< The object manager needs access to manage objects.

  private:

    BoundingSphere m_Sphere; ///< Bounding sphere.
    BoundingBox m_Aabb; ///< Axially aligned bounding box.

	int m_iUniqueID = 0;	//Unique ID for all objects. Only used for guards so far.

	float m_fFrameTimer = 0.0f; ///< Last time the frame was changed.
	float m_fFrameInterval = 0.1f; ///< Interval between frames.

    float m_fSpeed = 0.0f; ///< Speed.
    float m_fRotSpeed = 0.0f; ///< Rotational speed.
	int facingDir = 0;

	int objHealth = 3;
	float lifetime = 0.0f;

    Vector2 m_vOldPos = Vector2::Zero; ///< Last position.
    Vector2 m_vVelocity = Vector2::Zero; ///< Velocity.
    bool m_bDead = false; ///< Is dead or not.

    Vector2 m_vRadius = Vector2::Zero; ///< Half width and height of object sprite.

	//Tracks the time after an object has died. Used for the player to count the exact time when it died.
	float timeOfDeath = 0.0f;

  public:
	CObject();
    CObject(eSpriteType t, const Vector2& p); ///< Constructor.

    void move(); ///< Move object.
	void animator(); ///< Move object.

    void kill(); ///< Kill me.
	void killFX(); ///< Kill FX.
    bool IsDead(); ///< Query whether dead.
    void DeathFX(); ///< Death special effects.
	void DeathExplosionFX();
	void CollisionResponse(); ///< Collision response.
	void CollisionResponse(const BoundingBox& b); ///< Collision response between an object and walls
	void CollisionResponselevelTransfer(); //special behavior for level transfers
    
    void SetSpeed(float speed); ///< Set speed.
    float GetSpeed(); ///< Get speed.
    void SetRotSpeed(float speed); ///< Set rotational velocity.
	float GetRotSpeed();

	void takeDamage(const Vector2& damageDir, int hurtAmount);
	void setHealth(int newHealth) { objHealth = newHealth; }
	int getHealth() { return objHealth; }

    Vector2 GetViewVector(); //Get view vector.

    float GetOrientation(); ///< Get orientation.
    void SetOrientation(float angle); ///< Set orientation.
	void setFacing(int f) { facingDir = f; }
	int getFacing() { return facingDir; }
    
    const Vector2& GetVelocity(); ///< Get orientation.
    void SetVelocity(const Vector2& v); ///< Set orientation.
    
    const BoundingBox& GetBoundingBox(); ///< Get AABB.
    const BoundingSphere& GetBoundingSphere(); ///< Get bounding sphere.
    const Vector2& GetPos(); ///< Get position.

	float GetTimeOfDeath();

	void SetUniqueID(int i) { m_iUniqueID = i;};
	int GetUniqueID() { return m_iUniqueID; };
}; //CObject