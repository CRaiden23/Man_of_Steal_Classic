/// \file Guard.h
/// \brief Interface for the game object class CGuard.

#pragma once

#include "Object.h"
#include "Player.h"
#include "Guard.h"

/// \brief The game object. 
///
/// CGuard is the abstract representation of an object.

class CCamera : public CObject {
	friend class CObjectManager; ///< The object manager needs access to manage objects, including the player.

private:
	eStateType m_eState = PATROL_STATE;

	int lastPatrolPoint = 0;
	int numPatrolPoints = 2;

	Vector2 m_vDest = Vector2::Zero;
	Vector2 m_vLastSeen = Vector2::Zero;

	vector<CGuard*> visibleGuards;

	float m_fLastAiTime = 0.0f; ///< Last time AI was used.
	float m_fAiDelayTime = 0.0f; ///< Time until AI next used.

	float m_fDesiredRoll = 0; ///< Desired amount of roll.
	bool m_bRolling = false; ///< Object is rolling.
	float m_fSlerpControlFactor = 0; ///< Slerp control factor in [0, 1].

	void RotateToFacePoint(const Vector2& p);

	void changeState(eStateType newState);
	void think();
	void patrolAI();
	void idleAI();
	void alertAI();

public:
	CCamera(); ///< default Constructor
	CCamera(const Vector2& p); ///< Constructor

	void setDestination(const Vector2& d) { m_vDest = d; }

	void incLastPatrolPt() { lastPatrolPoint++; }
	int getNextPatrolPoint() { return ((lastPatrolPoint + 1) % numPatrolPoints) + 1; }

	void alertGuards();

	void move();
	void setRoll(float roll) { m_fRoll = roll; }
}; //CGuard