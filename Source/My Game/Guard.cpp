
#include "Guard.h"
#include "ComponentIncludes.h"
#include "GameDefines.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "TileManager.h"
#include "ObjectManager.h"
#include "Map.h"
#include "Quaternions.h"
#include "Utilities.h"

/// Create and initialize an object given its sprite type
/// and initial position.
/// \param t Type of sprite.
/// \param p Initial position of object.

CGuard::CGuard() : CGuard(Vector2::Zero) {
}

CGuard::CGuard(const Vector2& p) : CObject(GUARD_SPRITE, p){
} //constructor

CGuard::CGuard(const Vector2& p, enum eSpriteType t) : CObject(t, p) {
} //constructor for Armed Guards


CAGuard::CAGuard() : CAGuard(Vector2::Zero) {
}

CAGuard::CAGuard(const Vector2& p) : CGuard(p,ARMEDGUARD_SPRITE) {
	SetSpeed(0.0f);
} //constructor


CBossGrenadier::CBossGrenadier() : CBossGrenadier(Vector2::Zero) {
} //constructor

CBossGrenadier::CBossGrenadier(const Vector2& p) : CGuard(p,BOSS_GRENADIER_SPRITE) {
	m_fFacingAngle = DegreesToRadians(215.0f); //Left facing
	SetSpeed(0.0f);
	setHealth(BOSS_HEALTH);
}

void CGuard::move() {
	CObject::move();

	if (m_nSpriteIndex == GUARD_SPRITE)
		RotateToFacePoint(m_vDest);

	think();
}

void CAGuard::move() {
	CGuard::move();
	RotateToFacePoint(m_vDest);
}

void CBossGrenadier::move() {
	CGuard::move();
	RotateToFacePoint(m_vDest);
}

void CGuard::gotoClosestPatrolPoint() {
	bool visible = true;
	patrolPoints* possiblePP;
	possiblePP = m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->possiblePatrolPoints.at(GetUniqueID());

	for (int p = 0; p < numPatrolPoints; p++)
	{
		if (m_pTileManager->Visible(m_vPos, possiblePP->ptPoints.at(p), 5.0f))
		{
			setDestination(possiblePP->ptPoints.at(p));
		}
			
	}	
}

void CGuard::RotateToFacePoint(const Vector2& p) {
	const float r = m_pPlayer->getSphereRad();
	bool bVisible = m_pTileManager->Visible(m_vPos, p, r);

	if (bVisible) { //get desired roll
		const Vector2 v = p - m_vPos;
		m_fDesiredRoll = atan2(v.y, v.x) - XM_PI / 2.0f;
	} //if

	if (m_bRolling) {
		const float fDeltaRoll = fabs(m_fFacingAngle - m_fDesiredRoll); //amount of rolling

		if (fDeltaRoll < XM_PI / 16) { //small amount of roll
			m_fFacingAngle = m_fDesiredRoll; //snap to it
			m_bRolling = false; //done rolling
		} //if

		else { //large amount of roll, use quaternion slerp
			Quaternion q0 = QuaternionFromRoll(m_fFacingAngle); //rolling from
			Quaternion q1 = QuaternionFromRoll(m_fDesiredRoll); //rolling to

			Quaternion q = XMQuaternionSlerp(q0, q1, m_fSlerpControlFactor); //current roll

			m_fFacingAngle = RollFromQuaternion(q); //get roll from current quaternion

			const float t = m_pStepTimer->GetElapsedSeconds();
			const unsigned n = unsigned(16.0f * fDeltaRoll / (XM_2PI * t)); //number of roll increments
			m_fSlerpControlFactor += 1.0f / n; //increment roll control
		} //else
	} //if

	else if (bVisible) { //not rolling but visible
		m_bRolling = true; //start rolling
		m_fSlerpControlFactor = 0; //from zero
	} //else if
} //RotateToFacePoint

void CGuard::think() {

	if (m_pStepTimer->GetTotalSeconds() > m_fLastAiTime + m_fAiDelayTime) { // if we're able to change state again
		const float r = m_pPlayer->getSphereRad();
		bool bVisible = false;

		if(m_eState == ALERT_STATE)
			bVisible = m_pTileManager->Visible(m_vPos, m_pPlayer->m_vPos, r, m_fFacingAngle + XM_PIDIV2, XM_PIDIV2, 500.0f);
		else
			bVisible = m_pTileManager->Visible(m_vPos, m_pPlayer->m_vPos, r, m_fFacingAngle + XM_PIDIV2, XM_PIDIV4, 500.0f);

		m_fLastAiTime = m_pStepTimer->GetTotalSeconds();

		if (bVisible && m_eState != ALERT_STATE && m_eState != HURT_STATE) // if the player is visible and we're in any other state that's not alert or hurt
		{
			//Set boss to active now
			if (m_nSpriteIndex == BOSS_GRENADIER_SPRITE && !bIsBossActive) // Play once if the boss
			{
				CParticleDesc2D d;

				d.m_nSpriteIndex = ALERT_SPRITE;
				d.m_vPos = Vector2(m_vPos.x, m_vPos.y + 50);
				d.m_vVel = GetVelocity();
				d.m_fLifeSpan = 1.0f;
				d.m_fScaleInFrac = 0.4f;
				d.m_fFadeOutFrac = 0.5f;
				d.m_fMaxScale = 0.5f;

				m_pAudio->play(SPOT_SOUND);
				m_pParticleEngine->create(d);

				bIsBossActive = true;
			}
			else if (m_nSpriteIndex != BOSS_GRENADIER_SPRITE) // Play all the time as long as it's not the boss
			{
				CParticleDesc2D d;

				d.m_nSpriteIndex = ALERT_SPRITE;
				d.m_vPos = Vector2(m_vPos.x, m_vPos.y + 50);
				d.m_vVel = GetVelocity();
				d.m_fLifeSpan = 1.0f;
				d.m_fScaleInFrac = 0.4f;
				d.m_fFadeOutFrac = 0.5f;
				d.m_fMaxScale = 0.5f;

				m_pAudio->play(SPOT_SOUND);
				m_pParticleEngine->create(d);
			}
		}

		if (bVisible || m_eState == HURT_STATE) // if the player is visible, give chase
			changeState(ALERT_STATE);
		else if (!bVisible && m_eState == ALERT_STATE) // we just lost sight of the player
		{
			m_vLastSeen = m_pPlayer->m_vPos;
			changeState(CAUTION_STATE);
		}
		else // otherwise Patrol
		{
			if (m_nSpriteIndex != BOSS_GRENADIER_SPRITE)
				changeState(PATROL_STATE);
			else if (m_nSpriteIndex == BOSS_GRENADIER_SPRITE && !bIsBossActive)
				changeState(IDLE_STATE);
			else if (m_nSpriteIndex == BOSS_GRENADIER_SPRITE && bIsBossActive) //If the boss is active, stay in caution state instead
				changeState(CAUTION_STATE);
		}

		if (m_nSpriteIndex == GUARD_SPRITE)
		{
			if (sqrt(pow((m_vDest - m_vPos).x, 2) + pow((m_vDest - m_vPos).y, 2)) < 20 && m_eState == PATROL_STATE) // if we reach our destination
				changeState(IDLE_STATE);
		}
		else if (m_nSpriteIndex == ARMEDGUARD_SPRITE)
		{
			if (!m_bRolling && m_eState == PATROL_STATE) // if we reach our destination
				changeState(IDLE_STATE);
		}
		

		switch (m_eState) { //behavior depends on state
		case PATROL_STATE:
			patrolAI();
			break;
		case IDLE_STATE:
			idleAI();
			break;
		case CAUTION_STATE:
			cautionAI();
			break;
		case ALERT_STATE:
			alertAI();
			break;
		case HURT_STATE:
			alertAI();
			break;
		default:
			break;
		}
	}
}

void CGuard::changeState(eStateType newState) {
	m_eState = newState; //change state

	if (m_nSpriteIndex == ARMEDGUARD_SPRITE)
		SetSpeed(0.0f);
		
	switch (m_eState) { //change behavior settings
	case PATROL_STATE:
		m_fAiDelayTime = 0.2f * m_pRandom->randf();
		if (m_nSpriteIndex == GUARD_SPRITE)
			SetSpeed(50.0f);
		break;
	case IDLE_STATE:
		m_fAiDelayTime = 0.8f + 0.2f * m_pRandom->randf();
		if ( (m_nSpriteIndex == GUARD_SPRITE) ||
			 (m_nSpriteIndex == BOSS_GRENADIER_SPRITE)
			)
			SetSpeed(0.0f);
		break;
	case CAUTION_STATE:
		m_fAiDelayTime = 1.0f + 0.2f * m_pRandom->randf();
		if (m_nSpriteIndex == GUARD_SPRITE)
			SetSpeed(65.0f);
		else if (m_nSpriteIndex == BOSS_GRENADIER_SPRITE)
		{
			if (getHealth() < BOSS_HEALTH)
			SetSpeed(max(60.0f + (1 / (getHealth()) * 45.0f), 65.0f));
		}
		break;
	case ALERT_STATE:
		m_fAiDelayTime = 0.2f * m_pRandom->randf();
		if (m_nSpriteIndex == GUARD_SPRITE)
			SetSpeed(75.0f);
		else if (m_nSpriteIndex == BOSS_GRENADIER_SPRITE)
		{
			if ((EuclideanDistanceVectors(m_pPlayer->GetPos(), this->GetPos()) > 3.0f * 32))
				SetSpeed(0.0f);
			else
				SetSpeed(75.0f);
		}
		break;
	case HURT_STATE:
		m_fAiDelayTime = 0.25f + 0.2f * m_pRandom->randf();
			SetSpeed(200.0f);
		break;
	} //switch
}

void CGuard::CollisionResponse(const BoundingBox& b) {

	float w, h;
	m_pRenderer->GetSize(m_nSpriteIndex, w, h);
	w /= 2; h /= 2;

	const float top = b.Center.y + b.Extents.y;
	const float bottom = b.Center.y - b.Extents.y;

	const float left = b.Center.x - b.Extents.x;
	const float right = b.Center.x + b.Extents.x;

	if (m_vPos.y > top && m_vPos.y < top + h) {
		m_vPos.y = top + h;
		SetVelocity(Vector2(GetVelocity().x, 0));
	} //if

	else if (m_vPos.y < bottom && m_vPos.y > bottom - h) {
		m_vPos.y = bottom - h;
		SetVelocity(Vector2(GetVelocity().x, 0));
	} //else if
	
	if (m_vPos.x < left && m_vPos.x > left - w) {
		m_vPos.x = left - w - 1.0f;
		SetVelocity(Vector2(0, GetVelocity().y));
	} //else if

	else if (m_vPos.x > right && m_vPos.x < right + w) {
		m_vPos.x = right + w + 0.5f;
		SetVelocity(Vector2(0, GetVelocity().y));
	} //else if
} //CollisionResponse

void CGuard::takeDamage(const Vector2& damageDir, int hurtAmount)
{
	setHealth(getHealth()-hurtAmount); // decrement health
	

	printf("Guard %d: 'OOF!'\n", GetUniqueID());

	//push object away from damageDir
	Vector2 push = m_vPos - damageDir;
	if(hurtAmount != 0)
	switch (m_pRandom->randn(0, 4))
	{
	case 0:
		m_pAudio->play(EHURT1_SOUND);
		break;
	case 1:
		m_pAudio->play(EHURT2_SOUND);
		break;
	case 2:
		m_pAudio->play(EHURT3_SOUND);
		break;
	default:
		m_pAudio->play(EHURT4_SOUND);
		break;
	}
	//m_vPos += push;
	m_vDest = m_vPos + push;
	SetVelocity(m_vDest - m_vPos);

	if (m_nSpriteIndex == BOSS_GRENADIER_SPRITE)
	{
		if (getHealth() < BOSS_HEALTH && getHealth() != 0)
			this->m_f4Tint.x += max((1/getHealth()) * 1.4f, 0.75f);
	}

	changeState(HURT_STATE);
}

void CGuard::patrolAI() {

	//printf("Guard %d: 'Going to point: %d'\n", GetUniqueID(), getNextPatrolPoint());
	setDestination(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->possiblePatrolPoints.at(GetUniqueID())->ptPoints.at(getNextPatrolPoint()));

	if(m_nSpriteIndex == GUARD_SPRITE)
		SetVelocity(m_vDest - m_vPos);	
}

void CGuard::idleAI() {

	if (m_nSpriteIndex == BOSS_GRENADIER_SPRITE)	//Don't do anything else if boss
		return;
	//printf("Guard %d: 'Standing by at point: %d'\n", GetUniqueID(), getLastPatrolPoint());
	incLastPatrolPt();

	if (m_nSpriteIndex == ARMEDGUARD_SPRITE && getNextPatrolPoint() == 0)
		incLastPatrolPt();

	setDestination(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->possiblePatrolPoints.at(GetUniqueID())->ptPoints.at(getNextPatrolPoint()));

	if (m_nSpriteIndex == GUARD_SPRITE)
		SetVelocity(m_vDest - m_vPos);
}

void CGuard::cautionAI() {

	//printf("Guard %d: 'What was that?'\n", GetUniqueID());
	setDestination(m_vLastSeen);

	if (m_nSpriteIndex == GUARD_SPRITE || m_nSpriteIndex == BOSS_GRENADIER_SPRITE)
		SetVelocity(m_vDest - m_vPos);
}

void CGuard::alertAI() {

	//printf("Guard %d: 'ENEMY SIGHTED!!'\n", GetUniqueID());
	setDestination(m_pPlayer->m_vPos);

	if (m_nSpriteIndex == GUARD_SPRITE)
		SetVelocity(m_vDest - m_vPos);
	else if (m_nSpriteIndex == ARMEDGUARD_SPRITE)
	{
		if (m_pStepTimer->GetTotalSeconds() > m_fGunTimer + 1 && fabs(m_fFacingAngle - m_fDesiredRoll) < XM_PI / 8.0f) {
			m_fGunTimer = m_pStepTimer->GetTotalSeconds();
			m_pObjectManager->FireGun(this, BULLET2_SPRITE);
		}
	}
	else if (m_nSpriteIndex == BOSS_GRENADIER_SPRITE && m_eState != HURT_STATE)
	{
		float rateOfFire = 3.0f;

		//If the player is approaching us, run towards the player instead of blowing ourselves up
		if (EuclideanDistanceVectors(m_pPlayer->GetPos(), GetPos()) > 3.0f * 32)
		{
			// Calculate the new rate of fire
			if (getHealth() > 0)
			{
				//Get new speed
				rateOfFire = min((getHealth() * 0.65f), 3.0f);
			}

			if (m_pStepTimer->GetTotalSeconds() > m_fGunTimer + rateOfFire && fabs(m_fFacingAngle - m_fDesiredRoll) < XM_PI / 8.0f) {
				m_fGunTimer = m_pStepTimer->GetTotalSeconds();
				m_pObjectManager->FireGrenadeLauncher(this, BULLET_GRENADE_SPRITE);
			}
		}
		else
		{
			setDestination(m_pPlayer->m_vPos);
			SetVelocity(m_vDest - m_vPos);
		}
	}
}

void CAGuard::RotateToFacePoint(const Vector2& p) {
	const float r = m_pPlayer->getSphereRad();
	bool bVisible = m_pTileManager->Visible(m_vPos, p, r);

	if (bVisible) { //get desired roll
		const Vector2 v = p - m_vPos;
		m_fDesiredRoll = atan2(v.y, v.x) - XM_PIDIV2;
	} //if

	if (m_bRolling) {
		const float fDeltaRoll = fabs(m_fFacingAngle - m_fDesiredRoll); //amount of rolling

		if (fDeltaRoll < XM_PI / 32 || fDeltaRoll - XM_2PI < .01 && fDeltaRoll - XM_2PI > -.01) { //small amount of roll or a big amount, whatever
			m_fFacingAngle = m_fDesiredRoll; //snap to it
			m_bRolling = false; //done rolling
		} //if

		else { //large amount of roll, use quaternion slerp
			Quaternion q0 = QuaternionFromRoll(m_fFacingAngle); //rolling from
			Quaternion q1 = QuaternionFromRoll(m_fDesiredRoll); //rolling to

			Quaternion q = XMQuaternionSlerp(q0, q1, m_fSlerpControlFactor); //current roll

			m_fFacingAngle = RollFromQuaternion(q); //get roll from current quaternion

			const float t = m_pStepTimer->GetElapsedSeconds();
			const unsigned n = unsigned(16 * fDeltaRoll / (XM_2PI * t)); //number of roll increments
			m_fSlerpControlFactor += 1.0f / n; //increment roll control
		} //else
	} //if

	else if (bVisible) { //not rolling but visible
		m_bRolling = true; //start rolling
		m_fSlerpControlFactor = 0; //from zero
	} //else if
} //RotateToFacePoint

void CBossGrenadier::RotateToFacePoint(const Vector2& p) {
	const float r = m_pPlayer->getSphereRad();
	bool bVisible = m_pTileManager->Visible(m_vPos, p, r);

	if (bVisible) { //get desired roll
		const Vector2 v = p - m_vPos;
		m_fDesiredRoll = atan2(v.y, v.x) - XM_PIDIV2;
	} //if

	if (m_bRolling) {
		const float fDeltaRoll = fabs(m_fFacingAngle - m_fDesiredRoll); //amount of rolling

		if (fDeltaRoll < XM_PI / 32 || fDeltaRoll - XM_2PI < .01 && fDeltaRoll - XM_2PI > -.01) { //small amount of roll or a big amount, whatever
			m_fFacingAngle = m_fDesiredRoll; //snap to it
			m_bRolling = false; //done rolling
		} //if

		else { //large amount of roll, use quaternion slerp
			Quaternion q0 = QuaternionFromRoll(m_fFacingAngle); //rolling from
			Quaternion q1 = QuaternionFromRoll(m_fDesiredRoll); //rolling to

			Quaternion q = XMQuaternionSlerp(q0, q1, m_fSlerpControlFactor); //current roll

			m_fFacingAngle = RollFromQuaternion(q); //get roll from current quaternion

			const float t = m_pStepTimer->GetElapsedSeconds();
			const unsigned n = unsigned(16 * fDeltaRoll / (XM_2PI * t)); //number of roll increments
			m_fSlerpControlFactor += 1.0f / n; //increment roll control
		} //else
	} //if

	else if (bVisible) { //not rolling but visible
		m_bRolling = true; //start rolling
		m_fSlerpControlFactor = 0; //from zero
	} //else if
} //RotateToFacePoint
