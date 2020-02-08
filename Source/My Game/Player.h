/// \file Player.h
/// \brief Interface for the game object class CPlayer.

#pragma once

#include "Object.h"

/// \brief The game object. 
///
/// CPlayer is the abstract representation of an object.

class CPlayer : public CObject {
	friend class CObjectManager; ///< The object manager needs access to manage objects, including the player.

private:
	BoundingSphere m_Sphere; ///< Bounding sphere.
	BoundingBox m_Aabb; ///< Axially aligned bounding box.

	Vector2 m_vOldPos = Vector2::Zero; ///< Last position.

	Vector2 m_vRadius = Vector2::Zero; ///< Half width and height of object sprite.

	float playerSpeed = 75.0f;
	float speedMod = 1.0f;

	Vector2 playerSoundRadius = Vector2::Zero; // UNUSED
	float soundRadiusMod = 0.0f; // UNUSED

	bool attack = false;
	float m_fTimeofLastAtk = 0.0f;
	float m_fAtkCooldown = 0.5f; ///< Gun fire timer.

	float m_fSpeedControlFactor = 0.0f;

	bool isInvincible = false;
	float m_fInvincibleStart = 0.0f;
	float m_fInvincibilityTimer = 1.0f;
	Vector2 m_vKnockback = Vector2::Zero;

public:
	CPlayer(); ///< default Constructor
	CPlayer(const Vector2& p); ///< Constructor

	void move();

	const Vector2& getPlayerPos() { return m_vPos; }
	void setPlayerPos(const Vector2& pos) {	m_vPos = pos; }

	float getPSpeed() { return playerSpeed * speedMod; }

	bool isRunning() { if (speedMod > 1.20f) return true; else return false; }
	bool isCrawling() { if (speedMod < 0.9f) return true; else return false; }
	bool isAttacking() { return attack; }
	void setAttackState(bool newState) { attack = newState; }

	void changeSpeed(float desSpeed);
	void setSpeedMod(float m) { speedMod *= m; }

	void startInvincible(const Vector2& knockbackTo);
	bool isHurt() { return isInvincible; }

	void resetSpeedMod() {
		speedMod = 1.0f; 
		m_fSpeedControlFactor = 0.0f; 
	}

	void stepSound(float soundModifier);

	float getSphereRad() { return m_Sphere.Radius; }


}; //CPlayer