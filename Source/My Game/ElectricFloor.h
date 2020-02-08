#pragma once

#include "Object.h"

class CTrapElectric : public CObject {
	friend class CObjectManager; ///< The object manager needs access to manage objects, including the player.

private:
	enum eStateType {
		IDLE_STATE,
		KILL_STATE
	}; //eStateType

	enum CTrapElectric::eStateType m_eState = IDLE_STATE;

	BoundingBox m_Aabb; ///< Axially aligned bounding box.

	int guardID = 0; // figure out how to assign patrol points

	float m_fLastAiTime = 0.0f; ///< Last time AI was used.
	float m_fAiDelayTime = 0.0f; ///< Time until AI next used.

	int lID = 0;
	int rID = 0;

	bool m_bKillState = false;

	void changeState(eStateType newState);
	void CollisionResponse();
	void move();
	void think();

	void idleAI();
	void killAI();
public:
	CTrapElectric(); ///< default Constructor
	CTrapElectric(const Vector2& p); ///< Constructor

	const Vector2& getObjPos() { return m_vPos; }
	void setObjPos(const Vector2& pos) { m_vPos = pos; }

	void setWorldInfo(int level, int room) { lID = level; rID = room; }

	bool GetKillState() { return m_bKillState; };
	void SetKillState(bool newbool) { m_bKillState = newbool; };
}; //CGuard