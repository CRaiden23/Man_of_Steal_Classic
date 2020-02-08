
#include "Player.h"
#include "ComponentIncludes.h"
#include "GameDefines.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "helpers.h"
#include "Utilities.h"

/// Create and initialize an object given its sprite type
/// and initial position.
/// \param t Type of sprite.
/// \param p Initial position of object.

/// Move and update all bounding shapes.
/// The player object gets moved by the controller, everything
/// else moves an amount that depends on its velocity and the
/// frame time.

CPlayer::CPlayer():CPlayer(Vector2::Zero){
}

CPlayer::CPlayer(const Vector2& p) : CObject(PLAYER_SPRITE, p) {
} //constructor

void CPlayer::move() {

	const float t = m_pStepTimer->GetElapsedSeconds();

	Vector2 p = GetVelocity();

	p.Normalize();
	if (!isInvincible)
	{
		this->m_f4Tint.x = 1.0f;
		m_vPos.x += m_pPlayer->getPSpeed() * t * p.x;
		m_vPos.y += m_pPlayer->getPSpeed() * t * p.y;
	}
	else // if we are invincible
	{
		if(getHealth() != 0)
			this->m_f4Tint.x = max(3.0f * 1.4f, 0.75f);

		//printf("DIST = %f\n", EuclideanDistanceVectors(m_vPos, m_vKnockback));
		//printf("MAG = %f\n", sqrt((pow(m_vKnockback.x, 2) + pow(m_vKnockback.y, 2))));
		if ( (sqrt((pow(m_vKnockback.x, 2) + pow(m_vKnockback.y, 2))) > EuclideanDistanceVectors(m_vPos, m_vKnockback) && EuclideanDistanceVectors(m_vPos, m_vKnockback) > 10.0f && EuclideanDistanceVectors(m_vPos, m_vKnockback) < 70.0f) ||
		     (sqrt((pow(m_vKnockback.x, 2) + pow(m_vKnockback.y, 2))) < EuclideanDistanceVectors(m_vPos, m_vKnockback) ) ) // if we have NOT reached our knockback location
		{
			m_vPos.x += m_pPlayer->getPSpeed() * 5 * t * p.x;
			m_vPos.y += m_pPlayer->getPSpeed() * 5 * t * p.y;
		}
		else if ( (sqrt((pow(m_vKnockback.x, 2) + pow(m_vKnockback.y, 2))) > EuclideanDistanceVectors(m_vPos, m_vKnockback) && EuclideanDistanceVectors(m_vPos, m_vKnockback) >= 70 ) ) // we're waaay off course
		{
			m_vPos = m_vPos;
			m_vKnockback = Vector2::Zero;
			SetVelocity(Vector2::Zero);
			isInvincible = false;
		}
		else // we're really close to the knockback location
		{
			m_vPos = m_vKnockback;
			m_vKnockback = Vector2::Zero;
			SetVelocity(Vector2::Zero);
			isInvincible = false;
		}
	}

	if (isInvincible && m_pStepTimer->GetTotalSeconds() > m_fInvincibleStart + m_fInvincibilityTimer)
	{
		isInvincible = false;
	}
}

void CPlayer::startInvincible(const Vector2& knockbackTo)
{
	isInvincible = true;
	m_fInvincibleStart = m_pStepTimer->GetTotalSeconds();
	m_vKnockback = knockbackTo;
}

void CPlayer::changeSpeed(float desSpeed)
{
	speedMod = lerp(m_fSpeedControlFactor, speedMod, desSpeed);
	m_fSpeedControlFactor += 0.0002f;
}