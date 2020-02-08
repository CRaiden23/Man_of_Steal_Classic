#include "Gold.h"
#include "ComponentIncludes.h"
#include "GameDefines.h"
#include "Particle.h"
#include "ParticleEngine.h"

/// Create and initialize an object given its sprite type
/// and initial position.
/// \param t Type of sprite.
/// \param p Initial position of object.

/// Move and update all bounding shapes.
/// The player object gets moved by the controller, everything
/// else moves an amount that depends on its velocity and the
/// frame time.

CGold::CGold() :CGold(Vector2::Zero) {
}

CGold::CGold(const Vector2& p) : CObject(GOLD_SPRITE,p) {
} //constructor

void CGold::Updates()
{

}

int CGold::getIndex()
{
	return m_nCurrentFrame;
}