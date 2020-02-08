/// \file Common.h
/// \brief Interface for the class CCommon.

#pragma once

#include "SndList.h"

//forward declarations to make the compiler less stroppy

class CObjectManager; 
class CRenderer;
class CParticleEngine2D;
class CObject;
class CMap;
class CTileManager;
class CPlayer;
class CUI;
class CMusic;
class CHistoryManager;

/// \brief The common variables class.
///
/// CCommon is a singleton class that encapsulates 
/// things that are common to different game components,
/// including game state variables.
/// Making it a singleton class means
/// that we can avoid passing its member variables
/// around as parameters, which makes the code
/// minisculely faster, and more importantly, reduces
/// function clutter.

class CCommon{
  protected:  
    static CRenderer* m_pRenderer; ///< Pointer to the renderer.
    static CObjectManager* m_pObjectManager; ///< Pointer to the object manager.
    static CParticleEngine2D* m_pParticleEngine; ///< Pointer to particle engine.  
	static CMap* m_pMapManager;	/// Pointer to Map Manager. It's here so it can feed data to the tile manager and allow the game to transfer rooms/levels.
	static CHistoryManager* m_pHistoryManager;

	//Moved here now since it's used in more than one place
	static CTileManager* m_pTileManager; ///< Pointer to tile manager.

	//Basic Music manager
	static CMusic* m_pMusicManager;

    static bool m_bDrawAABBs; ///< Whether to draw AABBs.
	static bool bDrawControlScreen; ///< Draws controls screen 

	static int m_nCurLevelID; ///< Current level number.
	static int m_nCurRoomID;///< Keeps track of the current level / room the player is in
	static enum eGameplayState currentGameplayState;

	static XMVECTORF32 BackgroundColor;
	static XMFLOAT4 roomColor;
	static CSoundDesc  CurrentTrack;


    static Vector2 m_vWorldSize; ///< World height and width.
    static CPlayer* m_pPlayer; ///< Pointer to player character.
	static CUI* m_pUI; //Pointer to player action UI.
	static CUI* m_pBUI; //Pointer to box UI.
	static CUI* m_pHPUI; //Pointer to player HP UI.
	static CUI* m_pNUMA;
	static CUI* m_pNUMB;
	static CUI* m_pNUMC;
	static CUI* m_pNUMD;
	static CUI* m_pSCORE;
	static CUI* m_pVISION;
	static CUI* m_pBATTERY;
	static CUI* m_pSCOREUI;
	static CUI* m_pPOINTER;
}; //CCommon
