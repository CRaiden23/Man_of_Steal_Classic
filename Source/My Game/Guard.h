/// \file Guard.h
/// \brief Interface for the game object class CGuard.

#pragma once

#include "Object.h"
#include "Player.h"

/// \brief The game object. 
///
/// CGuard is the abstract representation of an object.

const int BOSS_HEALTH = 3; // multiply (or add) by difficulty modifier

class CGuard : public CObject {
	friend class CObjectManager; ///< The object manager needs access to manage objects, including the guards.

protected:
	eStateType m_eState = PATROL_STATE;

	CPlayer* m_pPlayer = nullptr;
	
	int lastPatrolPoint = 0;
	int numPatrolPoints = 0;

	float m_fFacingAngle = 0.0f;
	
	Vector2 m_vDest = Vector2::Zero;
	Vector2 m_vLastSeen = Vector2::Zero;
	bool bIsBossActive = false;

	float m_fLastAiTime = 0.0f; ///< Last time AI was used.
	float m_fAiDelayTime = 0.0f; ///< Time until AI next used.

	float m_fDesiredRoll = 0; ///< Desired amount of roll.
	bool m_bRolling = false; ///< Object is rolling.
	float m_fSlerpControlFactor = 0; ///< Slerp control factor in [0, 1].

	float m_fGunTimer = 0.0f;
	float fTimeSinceBossDeath = 0.0f;

	void RotateToFacePoint(const Vector2& p);

	void think();
	void patrolAI();
	void idleAI();
	void cautionAI();
	void alertAI();

	void takeDamage(const Vector2& damageDir, int hurtAmount);
	void CollisionResponse(const BoundingBox& b);
	void gotoClosestPatrolPoint();

public:
	CGuard(); ///< default Constructor
	CGuard(const Vector2& p); ///< Constructor
	CGuard(const Vector2& p, enum eSpriteType t); ///< Constructor for alt guards

	void changeState(eStateType newState);
	enum eStateType getState() { return m_eState; };

	void incLastPatrolPt() { lastPatrolPoint++; }

	void setDestination(const Vector2& d) { m_vDest = d; }

	int getNextPatrolPoint() { return ((lastPatrolPoint + 1) % numPatrolPoints); }
	int getLastPatrolPoint() { return (lastPatrolPoint % numPatrolPoints); }
	void setLastPatrolPoint(int ptPoint) { lastPatrolPoint = ptPoint; }
	void setNumPatrolPoints(int num) { numPatrolPoints = num; }

	const Vector2& getGuardPos() { return m_vPos; }
	void setGuardPos(const Vector2& pos) { m_vPos = pos; }

	void SetTimeOfDeathBoss(float newtime) { fTimeSinceBossDeath = newtime; };
	float GetTimeOfDeathBoss() { return fTimeSinceBossDeath; };

	void setPlayerPointer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	bool GetIsActive() { return bIsBossActive; };
	void move();
	void setFacingAngle(float fAngle) { m_fFacingAngle = fAngle; }
	float getFacingAngle() { return m_fFacingAngle; }
}; //CGuard

class CAGuard : public CGuard {
	friend class CObjectManager; ///< The object manager needs access to manage objects, including the guards.
public:
	CAGuard(); ///< default Constructor
	CAGuard(const Vector2& p); ///< Constructor

	void move();
private:
	void RotateToFacePoint(const Vector2& p);
};

class CBossGrenadier : public CGuard {
	friend class CObjectManager;
public:
	CBossGrenadier();
	CBossGrenadier(const Vector2& p);

	void move();
private:
	void RotateToFacePoint(const Vector2& p);
};

