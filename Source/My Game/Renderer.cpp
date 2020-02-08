/// \file Renderer.cpp
/// \brief Code for the renderer class CRenderer.

#include "Renderer.h"
#include "ComponentIncludes.h"
#include "Abort.h"
#include <Helpers.h>

CRenderer::CRenderer():
  CSpriteRenderer(Batched2D){
} //constructor

/// Load the specific images needed for this game.
/// This is where eSpriteType values from GameDefines.h get
/// tied to the names of sprite tags in gamesettings.xml. Those
/// sprite tags contain the name of the corresponding image file.
/// If the image tag or the image file are missing, then
/// the game should abort from deeper in the Engine code,
/// leaving you with a dialog box that tells you what
/// went wrong.

void CRenderer::LoadImages(){  
  BeginResourceUpload();

  Load(TILE_SPRITE, "tile"); 
  Load(PLAYER_SPRITE, "player");
  Load(TRAP_PITFALL_SPRITE, "trap_pitfall");
  Load(TRAP_ELECTRIC_SPRITE, "trap_electric");
  Load(BOSS_GRENADIER_SPRITE, "armedguard");

  Load(GREENLINE_SPRITE, "greenline");
  Load(BLUELINE_SPRITE, "blueline");
  Load(PUNCH_SPRITE, "bullet");
  Load(BULLET2_SPRITE, "bullet2");
  Load(BULLET_GRENADE_SPRITE, "bulletG");
  Load(SMOKE_SPRITE, "smoke");
  Load(ALERT_SPRITE, "alert");
  Load(EXPLOSION_SPRITE, "explosion");
  Load(EXPLOSION_FAKE_SPRITE, "explosion");

  Load(GUARD_SPRITE, "guard");
  Load(ARMEDGUARD_SPRITE, "armedguard");
  Load(CAMERA_SPRITE, "camera");


  Load(MOVEUI_SPRITE, "moveUI");
  Load(BOXUI_SPRITE, "boxUI");
  Load(HPUI_SPRITE, "hpUI");

  Load(GOLD_SPRITE, "gold");

  Load(NUMA_SPRITE, "numA");
  Load(NUMB_SPRITE, "numB");
  Load(NUMC_SPRITE, "numC");
  Load(NUMD_SPRITE, "numD");

  Load(UI_MSG_DEATH, "msg_dead");
  Load(UI_TUT_CONTROLS, "ui_controls");
  Load(UI_SCREEN_TITLE, "ui_screen_title");

  Load(NVISION_SPRITE, "nightVision");
  Load(NVGSCREEN_SPRITE, "nightVisionRedux");
  Load(BATTERY_SPRITE, "battery");
  Load(SCOREUI_SPRITE, "scoreUI");
  Load(BATTERYP_SPRITE, "batteryP");
  Load(POINTER_SPRITE, "pointer");

  // Maps
  /* Be sure to add to CMap::GetImageEnum() as well or this won't work! */

  // MANSION
  Load(MSN_ROOM_START, "msn_room_start");			// 0
  Load(MSN_ROOM_LOBBY, "msn_room_lobby");			// 1
  Load(MSN_ROOM_PATIO, "msn_room_patio");			// 2
  Load(MSN_ROOM_LIBRARY, "msn_room_library");		// 3
  Load(MSN_ROOM_HALLWAY1, "msn_room_hallway1");		// 4
  Load(MSN_ROOM_MUSEUM, "msn_room_museum");			// 5
  Load(MSN_ROOM_VAULT, "msn_room_vault");			// 6
  Load(MSN_ROOM_RIVERSIDE, "msn_room_riverside");	// 7
  Load(MSN_ROOM_QUARTERS, "msn_room_quarters");		// 8
  Load(MSN_ROOM_TUNNELS1, "msn_room_tunnels1");		// 9
  Load(MSN_ROOM_TUNNELS2, "msn_room_tunnels2");		// 10
  Load(MSN_ROOM_STORAGE, "msn_room_storage");		// 11

  EndResourceUpload();
} //LoadImages

/// Draw an axially aligned bounding box using the green line sprite.
/// \param aabb An axially aligned bounding box.

void CRenderer::DrawBoundingBox(const BoundingBox& aabb){
  const Vector3 center = aabb.Center;
  const Vector3 extent = aabb.Extents;
  
  const Vector2 topleft(center.x - extent.x, center.y + extent.y);
  const Vector2 topright(center.x + extent.x, center.y + extent.y);
  const Vector2 bottomleft(center.x - extent.x, center.y - extent.y);
  const Vector2 bottomright(center.x + extent.x, center.y - extent.y);
  
  DrawLine(GREENLINE_SPRITE, topleft, topright);
  DrawLine(GREENLINE_SPRITE, bottomleft, bottomright);
  DrawLine(GREENLINE_SPRITE, topleft, bottomleft);
  DrawLine(GREENLINE_SPRITE, topright, bottomright);
} //DrawBoundingBox

void CRenderer::DrawBlueBoundingBox(const BoundingBox& aabb) {
	const Vector3 center = aabb.Center;
	const Vector3 extent = aabb.Extents;

	const Vector2 topleft(center.x - extent.x, center.y + extent.y);
	const Vector2 topright(center.x + extent.x, center.y + extent.y);
	const Vector2 bottomleft(center.x - extent.x, center.y - extent.y);
	const Vector2 bottomright(center.x + extent.x, center.y - extent.y);

	DrawLine(BLUELINE_SPRITE, topleft, topright);
	DrawLine(BLUELINE_SPRITE, bottomleft, bottomright);
	DrawLine(BLUELINE_SPRITE, topleft, bottomleft);
	DrawLine(BLUELINE_SPRITE, topright, bottomright);

	
} //DrawBoundingBox

void CRenderer::DrawFOV(const Vector2& guard, float gFacing) {
	gFacing += XM_PIDIV2;
	
	const Vector2 fovL(guard.x + 100 * cos(gFacing + XM_PIDIV4), guard.y + 100 * sin(gFacing + XM_PIDIV4));
	const Vector2 fovR(guard.x + 100 * cos(gFacing - XM_PIDIV4), guard.y + 100 * sin(gFacing - XM_PIDIV4));

	DrawLine(BLUELINE_SPRITE, guard, fovL);
	DrawLine(BLUELINE_SPRITE, guard, fovR);

} //DrawFOV

RECT CRenderer::GetFontBoundingRect(const char* text)
{
	wchar_t* buffer = nullptr; //wide file name
	MakeWideFileName(text, buffer);
	return m_pFont->MeasureDrawBounds(buffer, XMFLOAT2(0.f, 0.f)); //bounding rectabgle
}
