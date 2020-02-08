#include "Pitfall.h"
#include "ComponentIncludes.h"

CTrapPitfall::CTrapPitfall()
{
	//Don't even move
	SetSpeed(0.0f);
}

CTrapPitfall::CTrapPitfall(const Vector2& p) : CObject(TRAP_PITFALL_SPRITE, p) {
	//Don't even move
	SetSpeed(0.0f);
	

} //constructor



void CTrapPitfall::changeState(eStateType newState)
{
	m_eState = newState; //change state

	switch (m_eState) { //change behavior settings
	case IDLE_STATE:
		break;
	case KILL_STATE:
			m_bTriggered = false;
			m_bKillState = true;
		break;
	case TRIGGERED_STATE:
			m_fAiDelayTime = 3.0f;		// After the trap activates due to collision response, change the collision response to kill the player.
			m_bTriggered = true;
		break;
	} //switch

	m_fLastAiTime = m_pStepTimer->GetTotalSeconds();
}

void CTrapPitfall::CollisionResponse() {

	if (!m_bTriggered && !m_bKillState)
	{
		m_pAudio->play(ETRAPPIT_CLICK);
		changeState(TRIGGERED_STATE);
	}

} //CollisionResponse

void CTrapPitfall::move() {
	think();
}


void CTrapPitfall::think()
{
	//Do something when the player triggered the trap
	if (m_bTriggered)
	{
		//Wait a couple of seconds before changing
		if (m_pStepTimer->GetTotalSeconds() > m_fLastAiTime + m_fAiDelayTime)
		{
				killAI();
				m_pAudio->play(ETRAPPIT_OPEN);
		}
	}
}

void CTrapPitfall::killAI()
{
	//Set to open state
	changeState(KILL_STATE);
	m_nCurrentFrame = 1;
}

