/// \file Renderer.h 
/// \brief Interface for the renderer class CRenderer.

#pragma once

#include "GameDefines.h"
#include "SpriteRenderer.h"

/// \brief The renderer.
///
/// CRenderer handles the game-specific rendering tasks, relying on
/// the base class to do all of the actual API-specific rendering.

class CRenderer: public CSpriteRenderer{
  public:
    CRenderer(); ///< Constructor.

	RECT GetFontBoundingRect(const char* text);
    void LoadImages(); ///< Load images.
    void DrawBoundingBox(const BoundingBox& aabb); ///< Draw AABB.
	void DrawBlueBoundingBox(const BoundingBox& aabb); ///< Draw AABB.
	void DrawFOV(const Vector2& guard, float gFacing);
}; //CRenderer
