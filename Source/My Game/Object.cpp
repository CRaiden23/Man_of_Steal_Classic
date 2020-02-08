/// \file Object.cpp
/// \brief Code for the game object class CObject.

#include "Object.h"
#include "Player.h"
#include "Guard.h"
#include "ComponentIncludes.h"
#include "GameDefines.h"
#include "Particle.h"
#include "ParticleEngine.h"
#include "Quaternions.h"
#include "Common.h"

#include "TileManager.h"
#include "Utilities.h"

/// Create and initialize an object given its sprite type
/// and initial position.
/// \param t Type of sprite.
/// \param p Initial position of object.
CObject::CObject() : CObject(PLAYER_SPRITE, Vector2::Zero) {
}

CObject::CObject(eSpriteType t, const Vector2& p){ 
  m_nSpriteIndex = t;
  m_vPos = m_vOldPos = p; 

  m_pRenderer->GetSize(t, m_vRadius.x, m_vRadius.y);
  m_vRadius *= 0.5f;
  
  const Vector2 topleft(-m_vRadius.x, m_vRadius.y);
  const Vector2 bottomrt(m_vRadius.x, -m_vRadius.y);
  BoundingBox::CreateFromPoints(m_Aabb, topleft + m_vPos, bottomrt + m_vPos);

  m_Sphere.Radius = max(m_vRadius.x, m_vRadius.y);
  m_Sphere.Center = (Vector3)m_vPos;

  const unsigned numframes = (unsigned)m_pRenderer->GetNumFrames(m_nSpriteIndex);
  if (numframes > 1)
	  m_nCurrentFrame = 0;

  m_fFrameInterval = 0.25f + 0.02f * m_pRandom->randf();

} //constructor

/// Move and update all bounding shapes.
/// The player object gets moved by the controller, everything
/// else moves an amount that depends on its velocity and the
/// frame time.

void CObject::move(){
  m_vOldPos = m_vPos;

  const float t = m_pStepTimer->GetElapsedSeconds();

  if(m_nSpriteIndex == PLAYER_SPRITE) {
	  if (!m_pPlayer->isAttacking())
		  m_pPlayer->move();
  }
  else
  {
	  m_vVelocity.Normalize();
	  m_vPos += m_vVelocity * t * m_fSpeed;

	  //m_fRoll = atan2(m_vVelocity.y, m_vVelocity.x) - XM_PI / 2.0f;
  }

  Vector2 p = GetVelocity();

  if (sqrt(pow(p.x, 2) + pow(p.y, 2)) > 0)
  {
	  if (((atan2(p.y, p.x) / XM_PIDIV2) * 3) == 0)
	  {
		  setFacing(fRIGHT);
	  }
	  else if (((atan2(p.y, p.x) / XM_PIDIV2) * 3) == 3)
	  {
		  setFacing(fBACK);
	  }
	  else if (((atan2(p.y, p.x) / XM_PIDIV2) * 3) == 6)
	  {
		  setFacing(fLEFT);
	  }
	  else if (((atan2(p.y, p.x) / XM_PIDIV2) * 3) == -3)
	  {
		  setFacing(fFRONT);
	  }
  }

  animator();

  //update bounding shapes

  const Vector2 topleft(-m_vRadius.x, m_vRadius.y);
  const Vector2 bottomrt(m_vRadius.x, -m_vRadius.y);
  BoundingBox::CreateFromPoints(m_Aabb, topleft + m_vPos, bottomrt + m_vPos);

  m_Sphere.Center = (Vector3)m_vPos;
} //move

void CObject::animator() {

	const size_t nFrameCount = m_pRenderer->GetNumFrames(m_nSpriteIndex);

	const float dt = 1000 * m_fFrameInterval / (1500 + fabs(m_vVelocity.x));
	
	switch(m_nSpriteIndex) {
	case PLAYER_SPRITE:
		if (nFrameCount > 1 && m_pStepTimer->GetTotalSeconds() > m_fFrameTimer + dt) { // if we have multiple frames and it's time to update it's frame
			if (m_pPlayer->isAttacking())
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 24 + facingDir/3;

				m_pPlayer->setAttackState(false);
			}
			else if (sqrt(pow(m_vVelocity.x, 2) + pow(m_vVelocity.y, 2)) == 0) // if our character is idle
			{
				if (m_pPlayer->isCrawling())
				{
					m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
					m_nCurrentFrame = 12 + facingDir + + 1;
				}
				else
				{
					m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
					m_nCurrentFrame = facingDir + 1;
				}
			}
			else if (m_pPlayer->isCrawling() && !m_pPlayer->isRunning()) // if the player is crawling
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 12 + facingDir + (m_nCurrentFrame + 1) % 3;
			}
			else if (!m_pPlayer->isCrawling()) // if the player is walking/running
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = facingDir + (m_nCurrentFrame + 1) % 3;
			}
		}
		break;
	case BOSS_GRENADIER_SPRITE:
	case ARMEDGUARD_SPRITE:
		if (nFrameCount > 1 && m_pStepTimer->GetTotalSeconds() > m_fFrameTimer + dt)
		{
			if (static_cast<CGuard*>(this)->getFacingAngle() > -XM_PIDIV2 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < -XM_PIDIV2 + XM_PIDIV4) // Right Facing
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 3;
			}
			else if (static_cast<CGuard*>(this)->getFacingAngle() > 0 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < 0 + XM_PIDIV4) // Back Facing
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 2;
			}
			else if (static_cast<CGuard*>(this)->getFacingAngle() > XM_PIDIV2 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < XM_PIDIV2 + XM_PIDIV4) // Left Facing
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 1;
			}
			else if (static_cast<CGuard*>(this)->getFacingAngle() > XM_PIDIV2 + XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() > XM_PI - XM_PIDIV4
				|| static_cast<CGuard*>(this)->getFacingAngle() < -XM_PIDIV2 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < -XM_PI + XM_PIDIV4) // Front Facing
			{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = 0;
			}
		}
	case GUARD_SPRITE:
		if (nFrameCount > 1 && m_pStepTimer->GetTotalSeconds() > m_fFrameTimer + dt)
		{
			Vector2 p = GetVelocity();

			if (GetSpeed() > 0) // if the guard is moving
			{
				if (static_cast<CGuard*>(this)->getFacingAngle() > -XM_PIDIV2 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < -XM_PIDIV2 + XM_PIDIV4) // Right Facing
				{
					m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
					m_nCurrentFrame = 6 + (m_nCurrentFrame + 1) % 2;
				}
				else if (static_cast<CGuard*>(this)->getFacingAngle() > 0 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < 0 + XM_PIDIV4) // Back Facing
				{
					m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
					m_nCurrentFrame = 4 + (m_nCurrentFrame + 1) % 2;
				}
				else if (static_cast<CGuard*>(this)->getFacingAngle() > XM_PIDIV2 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < XM_PIDIV2 + XM_PIDIV4) // Left Facing
				{
					m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
					m_nCurrentFrame = 2 + (m_nCurrentFrame + 1) % 2;
				}
				else if (static_cast<CGuard*>(this)->getFacingAngle() > XM_PIDIV2 + XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() > XM_PI - XM_PIDIV4 
					|| static_cast<CGuard*>(this)->getFacingAngle() < -XM_PIDIV2 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < -XM_PI + XM_PIDIV4) // Front Facing
				{
					m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
					m_nCurrentFrame = 0 + (m_nCurrentFrame + 1) % 2;
				}
			}
			else // if the guard is idle
			{
				if (static_cast<CGuard*>(this)->getFacingAngle() > -XM_PIDIV2 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < -XM_PIDIV2 + XM_PIDIV4) // Right Facing
				{
					m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
					m_nCurrentFrame = 6;
				}
				else if (static_cast<CGuard*>(this)->getFacingAngle() > 0 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < 0 + XM_PIDIV4) // Back Facing
				{
					m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
					m_nCurrentFrame = 4;
				}
				else if (static_cast<CGuard*>(this)->getFacingAngle() > XM_PIDIV2 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < XM_PIDIV2 + XM_PIDIV4) // Left Facing
				{
					m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
					m_nCurrentFrame = 2;
				}
				else if (static_cast<CGuard*>(this)->getFacingAngle() > XM_PIDIV2 + XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() > XM_PI - XM_PIDIV4
					|| static_cast<CGuard*>(this)->getFacingAngle() < -XM_PIDIV2 - XM_PIDIV4 && static_cast<CGuard*>(this)->getFacingAngle() < -XM_PI + XM_PIDIV4) // Front Facing
				{
					m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
					m_nCurrentFrame = 0;
				}
			}
		}
		break;
	default: // Update everything else's animations
		if (nFrameCount > 1 && m_pStepTimer->GetTotalSeconds() > m_fFrameTimer + dt) 
		{
				m_fFrameTimer = m_pStepTimer->GetTotalSeconds();
				m_nCurrentFrame = (m_nCurrentFrame + 1) % nFrameCount;
		}
		break;
	}
}

/// Respond to a collision by stopping and moving
/// back to the previous position, which is assumed
/// to be collision-free.
void CObject::CollisionResponse() {
	m_vPos = m_vOldPos;
}

void CObject::CollisionResponse(const BoundingBox& b){

	float w, h;
	m_pRenderer->GetSize(m_nSpriteIndex, w, h);
	w /= 2; h /= 2;
	
	const float top = b.Center.y + b.Extents.y;
	const float bottom = b.Center.y - b.Extents.y;

	const float left = b.Center.x - b.Extents.x;
	const float right = b.Center.x + b.Extents.x;

	if (m_vPos.y > top && m_vPos.y < top + h) {
		m_vPos.y = m_vOldPos.y;
		m_vVelocity.y = 0;
	} //if

	else if (m_vPos.y < bottom && m_vPos.y > bottom - h) {
		m_vPos.y = m_vOldPos.y;
		m_vVelocity.y = 0;
	} //else if

	if (m_vPos.x < left && m_vPos.x > left - w) {
		m_vPos.x = m_vOldPos.x;
		m_vVelocity.x = 0;
	} //else if

	else if (m_vPos.x > right && m_vPos.x < right + w) {
		m_vPos.x = m_vOldPos.x;
		m_vVelocity.x = 0;
	} //else if

} //CollisionResponse

// Perform a death particle effect to mark the death of an object.

void CObject::DeathFX(){
  CParticleDesc2D d;
  d.m_nSpriteIndex = SMOKE_SPRITE;
  d.m_vPos = m_vPos;
      
  d.m_fLifeSpan = 0.5f;
  d.m_fMaxScale = 0.5f;
  d.m_fScaleInFrac = 0.2f;
  d.m_fFadeOutFrac = 0.8f;
  d.m_fScaleOutFrac = d.m_fFadeOutFrac;

  m_pParticleEngine->create(d);
} //DeathFX

// Explosion effect for death
void CObject::DeathExplosionFX() {

} //DeathFX

/// Kill an object by marking its "is dead" flag. The object
/// will get deleted later at the appropriate time.

void CObject::takeDamage(const Vector2& damageDir, int hurtAmount)
{
	//push object away from damageDir
	Vector2 push = m_vPos - damageDir;

	if (m_nSpriteIndex == PLAYER_SPRITE)
	{
		if (m_pPlayer->isHurt())
			return;

		switch (m_pRandom->randn(0, 4))
		{
		case 0:
			m_pAudio->play(PHURT1_SOUND);
			break;
		case 1:
			m_pAudio->play(PHURT2_SOUND);
			break;
		case 2:
			m_pAudio->play(PHURT3_SOUND);
			break;
		default:
			m_pAudio->play(PHURT4_SOUND);
			break;
		}
		//m_vPos += push * .5;
		SetVelocity((m_vPos + push) - m_vPos);

		m_pPlayer->startInvincible(m_vPos + push);
	}
	else 
	{
		SetVelocity((m_vPos + push) - m_vPos);
	}	

	objHealth -= hurtAmount; // decrement health
}

void CObject::kill(){
  m_bDead = true;

  if (m_nSpriteIndex == BOSS_GRENADIER_SPRITE)
  {
	  DeathExplosionFX();
  }
  else
  {
	  DeathFX();
  }




  if (m_nSpriteIndex == PLAYER_SPRITE)
	  timeOfDeath = m_pStepTimer->GetTotalSeconds();
} //kill

//Same as above but only sets the "death" flag to true
void CObject::killFX(){
	m_bDead = true;
}


/// Reader function for the "is dead" flag.
/// \return true if marked as being dead, that is, ready for disposal.

bool CObject::IsDead(){
  return m_bDead;
} //IsDead

/// Set the object's speed, assuming that the object
/// moves according to its speed and view vector.
/// \param speed Speed.

void CObject::SetSpeed(float speed){
  m_fSpeed = speed;
} //SetVelocity

/// Set the object's rotational speed in revolutions per second.
/// \param speed Rotational speed in RPS.

void CObject::SetRotSpeed(float speed){
  m_fRotSpeed = speed;
} //Rotate

/// Reader function for the AABB.
/// \return The AABB.

const BoundingBox& CObject::GetBoundingBox(){
  return m_Aabb;
} //GetBoundingBox

/// Reader function for the bounding sphere.
/// \return The bounding sphere.

const BoundingSphere& CObject::GetBoundingSphere(){
  return m_Sphere;
} //GetBoundingSphere

/// Reader function for position.
/// \return Position.

const Vector2& CObject::GetPos(){
  return m_vPos;
} //GetPos

float CObject::GetTimeOfDeath()
{
	return timeOfDeath;
}

/// Reader function for speed.
/// \return Speed.

float CObject::GetSpeed(){
  return m_fSpeed;
} //

float CObject::GetRotSpeed() {
	return m_fRotSpeed;
} //

/// Get the view vector.
/// \return The view vector.

Vector2 CObject::GetViewVector(){
  return Vector2(-sin(m_fRoll), cos(m_fRoll));
} //GetViewVector

/// Reader function for the orientation. A 2D object's
/// orientation is its roll amount in 3D space.
/// \return The view vector.

float CObject::GetOrientation(){
  return m_fRoll;
} //GetOrientation

/// Writer function for the orientation. A 2D object's
/// orientation is its roll amount in 3D space.
/// \param angle The new orientation.

void CObject::SetOrientation(float angle){
  m_fRoll = angle;
} //GetOrientation

/// Reader function for velocity.
/// \return Velocity.

const Vector2& CObject::GetVelocity(){
  return m_vVelocity;
} //GetVel

/// Writer function for velocity.
/// \param v Velocity.

void CObject::SetVelocity(const Vector2& v){
  m_vVelocity = v;
} //SetVel


