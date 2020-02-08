#pragma once

#include "Object.h"

class CTrapPitfall : public CObject {
	friend class CObjectManager; ///< The object manager needs access to manage objects, including the player.

private:
	enum eStateType {
		IDLE_STATE,
		TRIGGERED_STATE,
		KILL_STATE
	}; //eStateType

	eStateType m_eState = IDLE_STATE;

	BoundingBox m_Aabb; ///< Axially aligned bounding box.

	int guardID = 0; // figure out how to assign patrol points

	float m_fLastAiTime = 0.0f; ///< Last time AI was used.
	float m_fAiDelayTime = 0.0f; ///< Time until AI next used.

	int lID = 0;
	int rID = 0;

	bool m_bTriggered = false; ///< If triggered, then set to true
	bool m_bKillState = false;

	void changeState(eStateType newState);
	void CollisionResponse();
	void move();
	void think();

	void idleAI();
	void alertAI();
	void killAI();
public:
	CTrapPitfall(); ///< default Constructor
	CTrapPitfall(const Vector2& p); ///< Constructor

	const Vector2& getObjPos() { return m_vPos; }
	void setObjPos(const Vector2& pos) { m_vPos = pos; }

	void setWorldInfo(int level, int room) { lID = level; rID = room; }

	bool GetKillState() { return m_bKillState; };
	bool GetTriggeredState() { return m_bTriggered; };
}; //CGuard