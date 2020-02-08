/// \file Common.cpp
/// \brief Code for the class CCommon.
///
/// This file contains declarations and initial values
/// for CCommon's static member variables.

#include "Common.h"

//Managers

CRenderer* CCommon::m_pRenderer					= nullptr;
CObjectManager* CCommon::m_pObjectManager		= nullptr;
CParticleEngine2D* CCommon::m_pParticleEngine	= nullptr;
CTileManager* CCommon::m_pTileManager			= nullptr;
CMap* CCommon::m_pMapManager					= nullptr;
CMusic* CCommon::m_pMusicManager				= nullptr;
CHistoryManager* CCommon::m_pHistoryManager		= nullptr;

//Variables

Vector2 CCommon::m_vWorldSize					= Vector2::Zero;
bool CCommon::m_bDrawAABBs						= false;
bool CCommon::bDrawControlScreen				= false;
int CCommon::m_nCurLevelID						= 0;
int CCommon::m_nCurRoomID						= 0;
enum eGameplayState CCommon::currentGameplayState;
XMVECTORF32 CCommon::BackgroundColor			= { 0.2f, 0.2f, 0.2f, 0.2f };

CSoundDesc  CCommon::CurrentTrack;

XMFLOAT4 CCommon::roomColor;

//Important Objects

CPlayer* CCommon::m_pPlayer						= nullptr;
CUI* CCommon::m_pUI								= nullptr;
CUI* CCommon::m_pBUI							= nullptr;
CUI* CCommon::m_pHPUI							= nullptr;
CUI* CCommon::m_pNUMA							= nullptr;
CUI* CCommon::m_pNUMB							= nullptr;
CUI* CCommon::m_pNUMC							= nullptr;
CUI* CCommon::m_pNUMD							= nullptr;
CUI* CCommon::m_pSCORE							= nullptr;
CUI* CCommon::m_pVISION							= nullptr;
CUI* CCommon::m_pBATTERY						= nullptr;
CUI* CCommon::m_pSCOREUI						= nullptr;
CUI* CCommon::m_pPOINTER						= nullptr;