/// \file ObjectManager.h
/// \brief Interface for the object manager CObjectManager.

#pragma once

#include <list>

#include "GameState.h"
#include "Object.h"
#include "Player.h"
#include "Guard.h"
#include "Camera.h"
#include "UI.h"
#include "Map.h"
#include "Component.h"
#include "Common.h"
#include "Settings.h"
#include "TileManager.h"
#include "Gold.h"
#include "Battery.h"
#include "HistoryManager.h"
#include "Pitfall.h"

using namespace std;

/// \brief The object manager.
///
/// A collection of all of the game objects.

class CObjectManager: 
  public CComponent, 
  public CCommon, 
  public CSettings{

  private:
    list<CObject*> m_stdObjectList; ///< Object list.

    void BroadPhase(); ///< Broad phase collision detection and response.
    void NarrowPhase(CObject* p0, CObject* p1); ///< Narrow phase collision detection and response.



    void CullDeadObjects(); ///< Cull dead objects.
	Vector2 oldPlayerPos;
	CGameState* tempGS; /// Temporary variable for game states. Do a deep copy before submitting to get a unique pointer
	enum eMusicState currentMusicState;
	float timeMusicStateChanged;
	float timeSinceGuardsAlerted;

  public:
    CObjectManager(); ///< Constructor.
    ~CObjectManager(); ///< Destructor.

    CObject* create(eSpriteType t, const Vector2& v); ///< Create new object.
	CObject* create(eSpriteType t, CObject* pObj, int rarity = 0); ///< Create new object.

	void createItemQuick(eSpriteType t, const Vector2& v, int rarity); ///< quicker way to create an object
	
    void clear(); ///< Reset to initial conditions.
    void move(int lID, int rID); ///< Move all objects.
    void draw(); ///< Draw all objects.
	void drawRoom(eSpriteType t); ///< Draw room separately.

	void IssueLevelTransferCall(Vector2 oldVectPos, int col_opcode);

	void FireGun(CObject* pObj, eSpriteType bullet);
	void FireGrenadeLauncher(CObject* pObj, eSpriteType bullet);
	void CreateExplosion(Vector2 pos, eSpriteType bullet);
	void BossExplosionDeath(Vector2 pos, eSpriteType explosion);
    void Punch(CObject* p, eSpriteType bullet); ///< Punch
	void CreateGameState();

	vector<CGuard*> findAllGuards();

	bool IsAGuardAlerted();

	bool IsBossAlerted();

	void ActiveElectricTraps();

	void DeactiveElectricTraps();

	void ManageGameplayState(enum eGameplayState State);

	// Main functions to submit gamestate to history
	void SubmitStateToHistory();

	// Deletes the Gamestate if called.
	void DropGameState();

	//Manages the current state of the game used for music
	void ManageMusicState(enum eMusicState musState);
	enum eMusicState GetMusicState() { return currentMusicState; };

	float GetTimeSinceMusicStateChanged() { return timeMusicStateChanged; };
	bool bMusicChanged = false;

	//Listener for level transfer
	bool bLevelTransfer = false;
	int iCollisionCode = -1;
	bool bBlockCreationOfGS = false;
}; //CObjectManager