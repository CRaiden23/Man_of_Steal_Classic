#include "ElectricFloor.h"
#include "ComponentIncludes.h"

CTrapElectric::CTrapElectric()
{
	//Don't even move
	SetSpeed(0.0f);
	m_bKillState = false;
}

CTrapElectric::CTrapElectric(const Vector2& p) : CObject(TRAP_ELECTRIC_SPRITE, p) {
	//Don't even move
	SetSpeed(0.0f);
	m_bKillState = false;

} //constructor



void CTrapElectric::changeState(eStateType newState)
{
	m_eState = newState; //change state

	switch (m_eState) { //change behavior settings
	case IDLE_STATE:
		m_bKillState = false;
		break;
	case KILL_STATE:
		m_bKillState = true;
		break;
	} //switch

	m_fLastAiTime = m_pStepTimer->GetTotalSeconds();
}

void CTrapElectric::CollisionResponse() {

} //CollisionResponse

void CTrapElectric::move() {
	if (m_bKillState)
		CObject::move();
	think();
}


void CTrapElectric::think()
{
	//Only do something if the trap is active
	if (m_bKillState)
		killAI();
	else
		idleAI();
}

void CTrapElectric::killAI()
{
	//Set to open state
	changeState(KILL_STATE);
}


void CTrapElectric::idleAI()
{
	//Set to open state
	changeState(IDLE_STATE);
	m_nCurrentFrame = 0;
}