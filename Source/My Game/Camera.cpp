
#include "Camera.h"
#include "Guard.h"
#include "ComponentIncludes.h"
#include "GameDefines.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "TileManager.h"
#include "ObjectManager.h"
#include "Map.h"
#include "Quaternions.h"

/// Create and initialize an object given its sprite type
/// and initial position.
/// \param t Type of sprite.
/// \param p Initial position of object.

CCamera::CCamera() : CCamera(Vector2::Zero) {
}

CCamera::CCamera(const Vector2& p) : CObject(CAMERA_SPRITE, p) { // should cameras keep track of all guards visible?
} //constructor

void CCamera::move() {
	CObject::move();
	RotateToFacePoint(m_vDest);
	think();
}

void CCamera::RotateToFacePoint(const Vector2& p) {
	const float r = m_pPlayer->getSphereRad();
	bool bVisible = m_pTileManager->Visible(m_vPos, p, r);

	if (bVisible) { //get desired roll
		const Vector2 v = p - m_vPos; 
		m_fDesiredRoll = atan2(v.y, v.x) - XM_PIDIV2;
	} //if

	if (m_bRolling) {
		const float fDeltaRoll = fabs(m_fRoll - m_fDesiredRoll); //amount of rolling

		if (fDeltaRoll < XM_PI / 32 || fDeltaRoll - XM_2PI < .01 && fDeltaRoll - XM_2PI > -.01) { //small amount of roll or a big amount, whatever
			m_fRoll = m_fDesiredRoll; //snap to it
			m_bRolling = false; //done rolling
		} //if

		else { //large amount of roll, use quaternion slerp
			Quaternion q0 = QuaternionFromRoll(m_fRoll); //rolling from
			Quaternion q1 = QuaternionFromRoll(m_fDesiredRoll); //rolling to

			Quaternion q = XMQuaternionSlerp(q0, q1, m_fSlerpControlFactor); //current roll

			m_fRoll = RollFromQuaternion(q); //get roll from current quaternion

			const float t = m_pStepTimer->GetElapsedSeconds();
			const unsigned n = unsigned(640 * fDeltaRoll / (XM_2PI * t)); //number of roll increments
			m_fSlerpControlFactor += 1.0f / n; //increment roll control
		} //else
	} //if

	else if (bVisible) { //not rolling but visible
		m_bRolling = true; //start rolling
		m_fSlerpControlFactor = 0; //from zero
	} //else if
} //RotateToFacePoint

void CCamera::think() {

	if (m_pStepTimer->GetTotalSeconds() > m_fLastAiTime + m_fAiDelayTime) { // if we're able to change state again
		const float r = m_pPlayer->getSphereRad();
		bool bVisible = m_pTileManager->Visible(m_vPos, m_pPlayer->m_vPos, r, m_fRoll + XM_PIDIV2, XM_PIDIV2, 200.0f);

		m_fLastAiTime = m_pStepTimer->GetTotalSeconds();

		if (bVisible && m_eState != ALERT_STATE) // if the player is visible and we're in any other state that's not alert or hurt
		{
			CParticleDesc2D d;

			d.m_nSpriteIndex = ALERT_SPRITE;
			d.m_vPos = Vector2(m_vPos.x, m_vPos.y + 20);
			d.m_vVel = GetVelocity();
			d.m_fLifeSpan = 1.0f;
			d.m_fScaleInFrac = 0.4f;
			d.m_fFadeOutFrac = 0.5f;
			d.m_fMaxScale = 0.5f;

			m_pAudio->play(SPOT_SOUND);
			m_pParticleEngine->create(d);

			alertGuards();
		}

		if (bVisible) // if the player is visible, give chase
			changeState(ALERT_STATE);
		else // otherwise Patrol
			changeState(PATROL_STATE);

		if (!m_bRolling && m_eState == PATROL_STATE) // if we reach our destination
			changeState(IDLE_STATE);

		switch (m_eState) { //behavior depends on state
		case PATROL_STATE:
			patrolAI();
			break;
		case IDLE_STATE:
			idleAI();
			break;
		case ALERT_STATE:
			alertAI();
			break;
		default:
			break;
		}
	}
}

void CCamera::changeState(eStateType newState) {
	m_eState = newState; //change state

	switch (m_eState) { //change behavior settings
	case PATROL_STATE:
		m_fAiDelayTime = 0.4f + 0.2f * m_pRandom->randf();
		SetSpeed(50.0f);
		break;
	case IDLE_STATE:
		m_fAiDelayTime = 2.0f + 0.2f * m_pRandom->randf();
		SetSpeed(0.0f);
		break;
	case ALERT_STATE:
		m_fAiDelayTime = 0.2f * m_pRandom->randf();
		SetSpeed(75.0f);
		break;
	} //switch
}

void CCamera::alertGuards() {
	for (auto const& p : m_pObjectManager->findAllGuards()) //for each object
	{
		p->changeState(ALERT_STATE);

		CParticleDesc2D d;

		d.m_nSpriteIndex = ALERT_SPRITE;
		d.m_vPos = Vector2(p->m_vPos.x, p->m_vPos.y + 20);
		d.m_vVel = GetVelocity();
		d.m_fLifeSpan = 1.0f;
		d.m_fScaleInFrac = 0.4f;
		d.m_fFadeOutFrac = 0.5f;
		d.m_fMaxScale = 0.5f;

		m_pAudio->play(SPOT_SOUND);
		m_pParticleEngine->create(d);
	}
}

void CCamera::patrolAI() {
	setDestination(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->possiblePatrolPoints.at(GetUniqueID())->ptPoints.at(getNextPatrolPoint()));
}

void CCamera::idleAI() {
	incLastPatrolPt();

	setDestination(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->possiblePatrolPoints.at(GetUniqueID())->ptPoints.at(getNextPatrolPoint()));
}

void CCamera::alertAI() {
	setDestination(m_pPlayer->m_vPos);
}