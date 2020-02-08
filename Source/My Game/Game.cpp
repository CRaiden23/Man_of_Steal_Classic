/// \file Game.cpp
/// \brief Code for the game class CGame.

#include "Game.h"
#include "GameDefines.h"
#include "Map.h"
#include "Music.h"
#include "Helpers.h"
#include "Renderer.h"
#include "ComponentIncludes.h"
#include "ParticleEngine.h"
#include "HistoryManager.h"
#include "ElectricFloor.h"

/// Delete the renderer and the object manager.

CGame::~CGame(){
  delete m_pParticleEngine;
  delete m_pRenderer;
  delete m_pObjectManager;
  delete m_pTileManager;
  delete m_pMapManager;
  delete m_pMusicManager;

} //destructor

/// Initialize the renderer and the object manager, load 
/// images and sounds, and begin the game.

void CGame::Initialize(){

  m_pRenderer = new CRenderer; 
  m_pRenderer->Initialize(NUM_SPRITES); 
  m_pRenderer->LoadImages(); //load images from xml file list


  m_pObjectManager = new CObjectManager; //set up the object manager 
  m_pAudio->Load(); //load the sounds for this game

  //Start up music manager and load sounds, same as m_pAudio
  m_pMusicManager = new CMusic;
  m_pMusicManager->Load();

  m_pTileManager = new CTileManager(32);
  m_pMapManager = new CMap;

  //Seed number generators
  m_pRandom->srand(time(NULL));
  srand(time(NULL));


  m_pHistoryManager = new CHistoryManager;

  m_pParticleEngine = new CParticleEngine2D((CSpriteRenderer*)m_pRenderer);

  currentGameplayState = STATE_SCREEN_MAINMENU;

//  BeginGame();
} //Initialize

/// Release all of the DirectX12 objects by deleting the renderer.

void CGame::Release(){
  delete m_pRenderer;
  m_pRenderer = nullptr; //for safety
} //Release

/// Ask the object manager to create a player object
/// and a turret.

void CGame::CreateObjects(){

  CreateTraps(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->possiblePatrolPoints.size()); // spawn traps

  m_pPlayer = (CPlayer*)m_pObjectManager->create(PLAYER_SPRITE, Vector2(500.0f, 64.0f)); // spawn player

  CreateGold(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.size());

  CreateBattery(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.size());

  CreateGuards(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->possiblePatrolPoints.size()); // spawn gaurds

  //Assign room color using information from level struct
  roomColor = m_pMapManager->GetLevel(m_nCurLevelID)->mapColor;

  //--------------ALWAYS ON TOP
  CreateUI();
  
} //CreateObjects

void CGame::CreateUI() {
	m_pVISION = (CUI*)m_pObjectManager->create(NVISION_SPRITE, Vector2(512.0f, 385.0f)); // night vision ui
	m_pPOINTER = (CUI*)m_pObjectManager->create(POINTER_SPRITE, Vector2(-50.0f, 190.0f)); // pointer UI
	m_pBUI = (CUI*)m_pObjectManager->create(BOXUI_SPRITE, Vector2(96.0f, 180.0f)); // box ui
	m_pUI = (CUI*)m_pObjectManager->create(MOVEUI_SPRITE, Vector2(94.0f, 60.0f)); // action ui
	m_pHPUI = (CUI*)m_pObjectManager->create(HPUI_SPRITE, Vector2(101.0f, 300.0f)); // hp ui
	m_pNUMA = (CUI*)m_pObjectManager->create(NUMA_SPRITE, Vector2(125.0f, 220.0f)); // ones place
	m_pNUMB = (CUI*)m_pObjectManager->create(NUMB_SPRITE, Vector2(105.0f, 220.0f)); // tens place
	m_pNUMC = (CUI*)m_pObjectManager->create(NUMC_SPRITE, Vector2(85.0f, 220.0f)); // hundreds place
	m_pNUMD = (CUI*)m_pObjectManager->create(NUMD_SPRITE, Vector2(65.0f, 220.0f)); // thousands place
	m_pSCORE = (CUI*)m_pObjectManager->create(NUMD_SPRITE, Vector2(-50.0f, -190.0f)); //keeps track of the score for each level
	m_pBATTERY = (CUI*)m_pObjectManager->create(BATTERY_SPRITE, Vector2(96.0f, 120.0f)); // battery ui
	m_pSCOREUI = (CUI*)m_pObjectManager->create(SCOREUI_SPRITE, Vector2(94.0f, 260.0f)); // score title ui

	//setDarkness();
}

void CGame::setDarkness()// sets certain rooms to be dark
{
	printf("\nCurrent Room ID: %d\n", m_nCurRoomID);
	if (m_pPOINTER->getDif() == 'E')
	{
		m_pVISION->setVisionL();
	}
	else if (m_pPOINTER->getDif() == 'H' && (m_nCurRoomID == 3 || m_nCurRoomID == 1 || m_nCurRoomID == 8 || m_nCurRoomID == 9 || m_nCurRoomID == 11 || m_nCurRoomID == 2 || m_nCurRoomID == 4))
	{
		m_pVISION->setVisionD();
	}
	else if (m_pPOINTER->getDif() == 'M')
	{
		m_pVISION->setVisionD();
	}
	else
		m_pVISION->setVisionL();
}

// To get around Painter's algorithm, this needs to be called first before the player is created.
void CGame::CreateTraps(int n)
{
	patrolPoints* possiblePP;
	for (int i = 0; i < n; i++)
	{
		//This should make this cleaner
		possiblePP = m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->possiblePatrolPoints.at(i);

		if (possiblePP->bIsTrapPitfall)
		{
			CTrapPitfall* pTrapP = (CTrapPitfall*)m_pObjectManager->create(TRAP_PITFALL_SPRITE, possiblePP->ptPoints.at(0));
			pTrapP->SetUniqueID(possiblePP->ptID);
		}
		else if (possiblePP->bIsTrapElectric)
		{
			CTrapElectric* pTrapE = (CTrapElectric*)m_pObjectManager->create(TRAP_ELECTRIC_SPRITE, possiblePP->ptPoints.at(0));
			pTrapE->SetUniqueID(possiblePP->ptID);
		}
	}
}

void CGame::CreateGuards(int n) {

	//Retrieve the list of dead guards
	vector<CGameState*> GS = m_pHistoryManager->RetrieveAllGameStatesByCriteria(m_nCurLevelID, m_nCurRoomID);
	vector<int>			deadGuards;
	patrolPoints*		possiblePP;
	bool				bCheckGuards = false;

	//Consolidate and retrieve all of the guards that died across all gamestates
 	for (auto const& p : GS)
		if (p->GetGuardsKilled().size() > 0)
			for (auto const& q : p->GetGuardsKilled())
			{
				deadGuards.push_back(*q); 
				bCheckGuards = true;		//We have to check if we need to add guards
			}

	for (int i = 0; i < n; i++)
	{
		//This should make this cleaner
		possiblePP = m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->possiblePatrolPoints.at(i);

		if (possiblePP->bIsCamera)
		{
			CCamera* pCam = (CCamera*)m_pObjectManager->create(CAMERA_SPRITE, possiblePP->ptPoints.at(0));
			pCam->setRoll(atan2((possiblePP->ptPoints.at(1) - possiblePP->ptPoints.at(0)).y, (possiblePP->ptPoints.at(1) - possiblePP->ptPoints.at(0)).x) - XM_PIDIV2);
			pCam->SetUniqueID(possiblePP->ptID);
			continue;
		}

		if (bCheckGuards)
		{
			bool bSpawnGuard = true;
			for (auto const& p : deadGuards)
			{
				//Check if this guard wasn't already killed
				if (p == possiblePP->ptID)
				{
					bSpawnGuard = false;
				}
			}

			if (bSpawnGuard)
			{
				if (possiblePP->bIsGuard)
				{
					int randomStart = m_pRandom->randn(0, possiblePP->ptPoints.size() - 2);

					CGuard* pGuard = (CGuard*)m_pObjectManager->create(GUARD_SPRITE, possiblePP->ptPoints.at(randomStart));
					pGuard->setFacingAngle(atan2((possiblePP->ptPoints.at(randomStart + 1) - possiblePP->ptPoints.at(randomStart)).y, (possiblePP->ptPoints.at(randomStart + 1) - possiblePP->ptPoints.at(randomStart)).x) - XM_PIDIV2);
					pGuard->setLastPatrolPoint(randomStart);
					pGuard->SetUniqueID(possiblePP->ptID);
					pGuard->setNumPatrolPoints(possiblePP->ptPoints.size());
					pGuard->setPlayerPointer(m_pPlayer);
				}
				else if (possiblePP->bIsArmedGuard)
				{
					CGuard* pGuard = (CGuard*)m_pObjectManager->create(ARMEDGUARD_SPRITE, possiblePP->ptPoints.at(0));
					pGuard->setFacingAngle(atan2((possiblePP->ptPoints.at(1) - possiblePP->ptPoints.at(0)).y, (possiblePP->ptPoints.at(0) - possiblePP->ptPoints.at(1)).x) - XM_PIDIV2);
					pGuard->SetUniqueID(possiblePP->ptID);
					pGuard->setNumPatrolPoints(possiblePP->ptPoints.size());
					pGuard->setPlayerPointer(m_pPlayer);
				}
				else if (possiblePP->bIsBossGrenadier)
				{
					CGuard* pGuard = (CGuard*)m_pObjectManager->create(BOSS_GRENADIER_SPRITE, possiblePP->ptPoints.at(0));
					pGuard->SetUniqueID(possiblePP->ptID);
					pGuard->setNumPatrolPoints(possiblePP->ptPoints.size());
					pGuard->setPlayerPointer(m_pPlayer);
				}				
			}
		}
		else             //We don't have to check for guards at the moment, add them
		{
			if (possiblePP->bIsGuard)
			{
				int randomStart = m_pRandom->randn(0, possiblePP->ptPoints.size() - 2);

				CGuard* pGuard = (CGuard*)m_pObjectManager->create(GUARD_SPRITE, possiblePP->ptPoints.at(randomStart));
				pGuard->setFacingAngle(atan2((possiblePP->ptPoints.at(randomStart + 1) - possiblePP->ptPoints.at(randomStart)).y, (possiblePP->ptPoints.at(randomStart + 1) - possiblePP->ptPoints.at(randomStart)).x) - XM_PIDIV2);
				pGuard->setLastPatrolPoint(randomStart);
				pGuard->SetUniqueID(possiblePP->ptID);
				pGuard->setNumPatrolPoints(possiblePP->ptPoints.size());
				pGuard->setPlayerPointer(m_pPlayer);
			}
			else if (possiblePP->bIsArmedGuard)
			{
				CGuard* pGuard = (CGuard*)m_pObjectManager->create(ARMEDGUARD_SPRITE, possiblePP->ptPoints.at(0));
				pGuard->setFacingAngle(atan2((possiblePP->ptPoints.at(1) - possiblePP->ptPoints.at(0)).y, (possiblePP->ptPoints.at(0) - possiblePP->ptPoints.at(1)).x) - XM_PIDIV2);
				pGuard->SetUniqueID(possiblePP->ptID);
				pGuard->setNumPatrolPoints(possiblePP->ptPoints.size());
				pGuard->setPlayerPointer(m_pPlayer);
			}
			else if (possiblePP->bIsBossGrenadier)
			{
				CGuard* pBossG = (CGuard*)m_pObjectManager->create(BOSS_GRENADIER_SPRITE, possiblePP->ptPoints.at(0));
				pBossG->SetUniqueID(possiblePP->ptID);
				pBossG->setNumPatrolPoints(possiblePP->ptPoints.size());
				pBossG->setPlayerPointer(m_pPlayer);
			}
		}
	}
}

void CGame::CreateBattery(int n) {
	printf("Creating %d Battery\n", n);

	//Retrieve the list of obtained batteries
	vector<CGameState*> GS = m_pHistoryManager->RetrieveAllGameStatesByCriteria(m_nCurLevelID, m_nCurRoomID);
	vector<int>			batteryCollected;
	bool				bCheckBattery = false;


	int					chances = 0;
	int					tempRand = 0;

	//Consolidate and retrieve all of the batteries that was retrieved across all gamestates
	for (auto const& p : GS)
		if (p->GetBatteryCollected().size() > 0)
			for (auto const& q : p->GetBatteryCollected())
			{
				batteryCollected.push_back(q->ID);
				bCheckBattery = true;
			}

	//Create gamestate object and record the state if battery hasn't spawn
	CGameState* newGameState = new CGameState(m_nCurLevelID, m_nCurRoomID);

	for (int i = 0; i < n; i++)
	{
		if (!(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(i)->bIsBattery))
			continue;
		if (bCheckBattery)
		{
			bool bSpawnBattery = true;
			for (auto const& p : batteryCollected)
			{
				if (p == m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(i)->uniqueID)
				{
					bSpawnBattery = false;
				}
			}

			if (bSpawnBattery)
				SpawnBatteryOnMap(i, newGameState);
		}
		else
		{
			SpawnBatteryOnMap(i, newGameState);
		}
	}

	m_pHistoryManager->SubmitGameState(newGameState);
}

void CGame::SpawnBatteryOnMap(int it, CGameState*& newGameState)
{
	int chances = 0;
	int tempRand = 0;

	chances = m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(it)->chance;
	tempRand = m_pRandom->randn(0, 100);
	if (tempRand <= chances)
	{
		CBattery* pBattery = (CBattery*)m_pObjectManager->create(BATTERYP_SPRITE, m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(it)->location);

		//Give Unique ID from map file
		pBattery->SetUniqueID(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(it)->uniqueID);

		pBattery->m_nCurrentFrame = 0;
	}
	else
	{
		newGameState->AddBatteryCollected(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(it)->uniqueID, 0);
	}
}

void CGame::CreateGold(int n) {
	printf("Creating %d Gold\n", n);

	//Retrieve the list of obtained gold
	vector<CGameState*> GS = m_pHistoryManager->RetrieveAllGameStatesByCriteria(m_nCurLevelID, m_nCurRoomID);
	vector<int>			goldCollected;
	bool				bCheckGold = false;


	int					chances = 0;
	int					tempRand = 0;

	//Consolidate and retrieve all of the gold that was retrieved across all gamestates
	for (auto const& p : GS)
		if (p->GetGoldCollected().size() > 0)
			for (auto const& q : p->GetGoldCollected())
			{
				goldCollected.push_back(q->ID); 
				bCheckGold = true;
			}

	//Create gamestate object and record the state if gold hasn't spawn
	CGameState* newGameState = new CGameState(m_nCurLevelID, m_nCurRoomID);

	for (int i = 0; i < n; i++) 
	{
		if (!(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(i)->bIsGold))
			continue;

		if (bCheckGold)
		{
			bool bSpawnGold = true;
			for (auto const& p : goldCollected)
			{
				if (p == m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(i)->uniqueID)
				{
					bSpawnGold = false;
				}
			}

			if (bSpawnGold)
				SpawnGoldOnMap(i, newGameState);
		}
		else
		{
			SpawnGoldOnMap(i, newGameState);
		}
	}

	m_pHistoryManager->SubmitGameState(newGameState);
}

/*

  Not to be confused with CreateGold(). This one actually creates the gold on the map!
  Attempts to spawn gold if chances is higher than or equal to tempRand

*/
void CGame::SpawnGoldOnMap(int it, CGameState* &newGameState)
{
	int chances = 0;
	int tempRand = 0;

	chances = m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(it)->chance;
	tempRand = m_pRandom->randn(1, 100);
	if (tempRand <= chances)
	{
		CGold* pGold = (CGold*)m_pObjectManager->create(GOLD_SPRITE, m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(it)->location);

		//Give Unique ID from map file
		pGold->SetUniqueID(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(it)->uniqueID);

		if (chances <= 100 && chances >= 91) // used to determine which gold sprite will appear in the map
		{
			chances = 0;
		}
		else if (chances <= 90 && chances >= 81)
		{
			chances = 1;
		}
		else if (chances <= 80 && chances >= 71)
		{
			chances = 2;
		}
		else if (chances <= 70 && chances >= 61)
		{
			chances = 3;
		}
		else if (chances <= 60 && chances >= 51)
		{
			chances = 4;
		}
		else if (chances <= 50 && chances >= 46)
		{
			chances = 5;
		}
		else if (chances <= 45 && chances >= 31)
		{
			chances = 6;
		}
		else if (chances <= 30 && chances >= 16)
		{
			chances = 7;
		}
		else if (chances <= 15 && chances >= 11)
		{
			chances = 8;
		}
		else if (chances <= 10 && chances >= 1)
		{
			chances = 9;
		}
		else
		{
			chances = 9;
		}


		pGold->m_nCurrentFrame = chances;
	}
	else
	{
		newGameState->AddGoldCollected(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.at(it)->uniqueID, 0);
	}
}

/// Call this function to start a new game. This
/// should be re-entrant so that you can start a
/// new game without having to shut down and restart the
/// program.

void CGame::BeginGame(char difficulty){  
  string file_map;

  //Start at 0,0
  m_nCurLevelID = 0;
  m_nCurRoomID = 0;

  m_pParticleEngine->clear(); //clear old particles
  m_pObjectManager->clear(); //clear old objects
  m_pObjectManager->bLevelTransfer = false;

  //Reset background color
  BackgroundColor = { 0.2f, 0.2f, 0.2f, 0.2f };

  CreateObjects(); //create new objects 
  m_pPOINTER->setDif(difficulty);
  setDarkness();

  //Create a new gamestate
  m_pObjectManager->CreateGameState();

  currentGameplayState = STATE_GAME_GREEN;

  file_map = m_pMapManager->GetFilePath(m_nCurLevelID, m_nCurRoomID);

  m_pTileManager->LoadMap(&file_map[0]); //load map

  //Set the music state to ambient
  m_pObjectManager->ManageMusicState(MUS_AMBIENT);

  CurrentTrack = m_pMusicManager->loop(m_pMapManager->GetSoundEnum(m_nCurLevelID,m_pObjectManager->GetMusicState()), 0.15f);

} //BeginGame

void CGame::NextLevel(){
	string filename;
	Vector2 newPosition;
	Vector2 lastPos;
	int lastHealth;
	int lastFacing;
	int lastScore;
	int lastBattery;
	char lastDifficulty;
	if (m_pVISION->getVision() == -1 || m_pVISION->getVision() == -2)
	{
		m_pAudio->play(NIGHTV2_SOUND);
	}

	//Reset to false
	m_pObjectManager->bLevelTransfer = false;

	lastPos = m_pPlayer->getPlayerPos(); //Get the player info before it gets destroyed
	lastHealth = m_pPlayer->getHealth();
	lastFacing = m_pPlayer->getFacing();
	lastScore = m_pSCORE->getScore();
	lastBattery = m_pBATTERY->getBattery();
	lastDifficulty = m_pPOINTER->getDif();

	//Submit all changes in the game to the History for safekeeping
	m_pObjectManager->SubmitStateToHistory();

	m_pParticleEngine->clear(); //clear old particles
	m_pObjectManager->clear(); //clear old objects


	m_nCurRoomID = m_pMapManager->GetNextRoomID(m_nCurLevelID, m_nCurRoomID, m_pObjectManager->iCollisionCode);

	//Freeze game and spawn m_pPOINTER UI element
	if (m_nCurRoomID == 999)
	{
		currentGameplayState = STATE_SCREEN_ENDLEVEL;

		//Disable drawing AABBs
		m_bDrawAABBs = false;

		//Create some UI elements
		m_pSCORE = (CUI*)m_pObjectManager->create(NUMD_SPRITE, Vector2(-50.0f, -190.0f)); //keeps track of the score for each level
		m_pPOINTER = (CUI*)m_pObjectManager->create(POINTER_SPRITE, Vector2(-50.0f, 200.0f)); // pointer UI

		m_pSCORE->setScore(lastScore);
		m_pPOINTER->setDif(lastDifficulty);
	}
	else
	{
		filename = m_pMapManager->GetFilePath(m_nCurLevelID, m_nCurRoomID);
		m_pTileManager->LoadMap(&filename[0]); //load map

		//Grab coordinates of the next level transfer collision box
		newPosition = m_pTileManager->GetLevelTransferBoundingBox(m_pObjectManager->iCollisionCode, lastPos);

		///Create objects once more
		CreateTraps(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->possiblePatrolPoints.size()); // spawn traps

		m_pPlayer = (CPlayer*)m_pObjectManager->create(PLAYER_SPRITE, newPosition);
		m_pPlayer->setHealth(lastHealth);
		m_pPlayer->setFacing(lastFacing);

		CreateGold(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.size());

		CreateBattery(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.size());

		CreateGuards(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->possiblePatrolPoints.size());

		CreateBattery(m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->itemLocations.size());

		//Create a new gamestate
		m_pObjectManager->CreateGameState();

		//--------------ALWAYS ON TOP
		CreateUI();
		m_pSCORE->setScore(lastScore);
		m_pPOINTER->setDif(lastDifficulty);
		setDarkness();
		m_pBATTERY->setBattery(lastBattery);
	}
} //NextLevel

/// Poll the keyboard state and respond to the
/// key presses that happened since the last frame.

void CGame::KeyboardHandler(){
	m_pKeyboard->GetState(); //get current keyboard state 

	if (m_pPlayer->isHurt())
		return;

	Vector2 dir;
	dir.y = m_pPlayer->GetVelocity().y;
	if (m_pKeyboard->TriggerDown('W'))
	{
		dir.y = 1.0f;
	}
	if (m_pKeyboard->TriggerDown('S'))
	{
		dir.y = -1.0f;
	}

	if (m_pKeyboard->TriggerUp('W') ^ m_pKeyboard->TriggerUp('S'))
		  dir.y = 0.0f;
	// Vertical Axis Handler
    
	dir.x = m_pPlayer->GetVelocity().x;
	if (m_pKeyboard->TriggerDown('D'))
	{
		dir.x = 1.0f;
	}
	if (m_pKeyboard->TriggerDown('A'))
	{
		dir.x = -1.0f;
	}

	if (m_pKeyboard->TriggerUp('A') ^ m_pKeyboard->TriggerUp('D'))
		  dir.x = 0.0f;
	// Horizontal Axis Handler

	if (m_pKeyboard->Down(VK_SHIFT))
		m_pPlayer->changeSpeed(2.0f);

	if (m_pKeyboard->Down(VK_CONTROL))
		m_pPlayer->changeSpeed(0.5f);

	if (m_pKeyboard->TriggerUp(VK_CONTROL) || m_pKeyboard->TriggerUp(VK_SHIFT))
	{
		m_pPlayer->resetSpeedMod();
	}

	if (m_pKeyboard->TriggerDown('N') && m_pBATTERY->getBattery() > 0)// activates nightvision and plays sounds
	{
		if (m_pVISION->getVision() == 1 || m_pVISION->getVision() == 2)
		{
			m_pAudio->play(NIGHTV_SOUND);
			m_pBATTERY->usingDec();
		}
		else
		{
			m_pAudio->play(NIGHTV2_SOUND);
		}
		m_pVISION->changeVision();
	}

	m_pPlayer->SetSpeed( clamp(m_pPlayer-> getPSpeed() * sqrt(pow(dir.x, 2) + pow(dir.y, 2)), -m_pPlayer->getPSpeed(), m_pPlayer->getPSpeed()) ); // Set Speed equal to the magnitude of the velocity vector | clamp a between b and c

	//if (sqrt(pow(dir.x, 2) + pow(dir.y, 2)) > 0) // if we have a magnitude greater than 0
	//	m_pPlayer->SetOrientation(atan2(dir.x, dir.y)); // update the angle of the player to be the inverse tangent of the velocity vector
	
	m_pPlayer->SetVelocity(dir); // record the player's velocity

  if(m_pKeyboard->TriggerDown(VK_SPACE))
    m_pObjectManager->Punch(m_pPlayer, PUNCH_SPRITE);

  if (m_pKeyboard->TriggerDown(VK_F6))
	  //Restart current level if the collision code is not -1 (no collision occurred)
	  if (!(m_pObjectManager->iCollisionCode == -1))
		  NextLevel();

  //Toggle key to show controls
  //If key is released, stop showing controls
  if (m_pKeyboard->TriggerDown(VK_TAB))
	  bDrawControlScreen = true;

  if (m_pKeyboard->TriggerUp(VK_TAB))
	  bDrawControlScreen = false;

  if(m_pKeyboard->TriggerDown(VK_F1))
    m_bDrawAABBs = !m_bDrawAABBs;

  if (m_pKeyboard->TriggerDown(VK_F2))
	  printf("Current Position: %f %f\n", m_pPlayer->getPlayerPos().x, m_pPlayer->getPlayerPos().y);

  if (m_pKeyboard->TriggerDown(VK_F3))
	printf("Closest Tile: %f %f\n", round(m_pPlayer->getPlayerPos().x/32) * 32, round(m_pPlayer->getPlayerPos().y/32) * 32);

} //KeyboardHandler

void CGame::ScoreboardHandler()
{
	if (m_pPOINTER->GetTimeScoreSubmitted() == 0.0f)
	{
		m_pKeyboard->GetState(); //get current keyboard state 
		if (m_pKeyboard->TriggerDown('W'))
		{
			if (m_pPOINTER->getPos().x == 390)// if the scoreboard should be displayed, it changes letter up
				m_pPOINTER->changeInitials(0, -1);
			else if (m_pPOINTER->getPos().x == 500)
				m_pPOINTER->changeInitials(1, -1);
			else if (m_pPOINTER->getPos().x == 610)
				m_pPOINTER->changeInitials(2, -1);
		}
		if (m_pKeyboard->TriggerDown('S'))
		{
			if (m_pPOINTER->getPos().x == 390)// if the scoreboard should be displayed, it changes letter down
				m_pPOINTER->changeInitials(0, 1);
			else if (m_pPOINTER->getPos().x == 500)
				m_pPOINTER->changeInitials(1, 1);
			else if (m_pPOINTER->getPos().x == 610)
				m_pPOINTER->changeInitials(2, 1);
		}

		if (m_pKeyboard->TriggerDown('D'))
		{
			if (m_pPOINTER->getPos().x == 390)// if the scoreboard should be displayed, it allows D to move right
				m_pPOINTER->changePos(Vector2(500.0f, 210.0f));
			else if (m_pPOINTER->getPos().x == 500)
				m_pPOINTER->changePos(Vector2(610.0f, 210.0f));
			else if (m_pPOINTER->getPos().x == 610)
				m_pPOINTER->changePos(Vector2(390.0f, 210.0f));
		}
		if (m_pKeyboard->TriggerDown('A'))
		{
			if (m_pPOINTER->getPos().x == 500)// if the scoreboard should be displayed, it allows A to move left
				m_pPOINTER->changePos(Vector2(390.0f, 210.0f));
			else if (m_pPOINTER->getPos().x == 390)
				m_pPOINTER->changePos(Vector2(610.0f, 210.0f));
			else if (m_pPOINTER->getPos().x == 610)
				m_pPOINTER->changePos(Vector2(500.0f, 210.0f));
		}

		//If the enter key is pressed and the time hasn't been set yet
		if (m_pKeyboard->TriggerDown(VK_RETURN))
		{
			m_pPOINTER->storeSort();
			m_pPOINTER->writeFile();

			m_pPOINTER->SetTimeScoreSubmitted(m_pStepTimer->GetTotalSeconds());
		}
	}
}

/// Poll the XBox controller state and respond to the
/// controls there.

void CGame::ControllerHandler(){
  if(!m_pController->IsConnected())return;

  /*m_pController->GetState(); //get state of controller's controls 

  m_pPlayer->SetSpeed(100*m_pController->GetRTrigger());
  m_pPlayer->SetRotSpeed(-2.0f*m_pController->GetRThumb().x);

  if(m_pController->GetButtonRSToggle())
    m_pObjectManager->Attack(m_pPlayer, BULLET_SPRITE);
  
  if(m_pController->GetButtonAToggle())
    NextLevel();
  
  if(m_pController->GetButtonBToggle())
    m_bDrawAABBs = !m_bDrawAABBs;

  if(m_pController->GetDPadRight())
    m_pPlayer->StrafeRight();
  
  if(m_pController->GetDPadLeft())
    m_pPlayer->StrafeLeft();

  if(m_pController->GetDPadDown())
    m_pPlayer->StrafeBack();*/
} //ControllerHandler

/// Ask the object manager to draw the game objects. RenderWorld
/// is notified of the start and end of the frame so
/// that it can let Direct3D do its pipelining jiggery-pokery.

void CGame::RenderFrame(){

  m_pRenderer->BeginFrame();
  //Draw background first
  m_pRenderer->SetBgColor(BackgroundColor);

  if (currentGameplayState == STATE_SCREEN_MAINMENU)   //Creates the Main menu screen
  {
	  UI_MainMenu();
  }
  else if (currentGameplayState == STATE_SCREEN_ENDLEVEL)   //Creates the End Level screen
  {
	  m_pObjectManager->draw();
	  UI_EndLevel();
  }
  else   //The actual game
  {
	  m_pObjectManager->drawRoom(m_pMapManager->GetImageEnum(m_nCurLevelID, m_nCurRoomID));
	  m_pObjectManager->draw();
	  m_pParticleEngine->Draw();
	  //m_pRenderer->EndFrame();

	  if (m_pPlayer->IsDead())  //Creates the game over screen
	  {
		  UI_GameOver();
	  }
  }

  m_pRenderer->EndFrame();
} //RenderFrame

/// Make the camera follow the player, but don't let it get
/// too close to the edge. Unless the world is smaller than
/// the window, in which case we center everything.

void CGame::FollowCamera(){
  Vector3 vCameraPos(m_pPlayer->GetPos()); //player position

  if(m_vWorldSize.x > m_nWinWidth){ //world wider than screen
    vCameraPos.x = max(vCameraPos.x, m_nWinWidth/2.0f); //stay away from the left edge
    vCameraPos.x = min(vCameraPos.x, m_vWorldSize.x - m_nWinWidth/2.0f);  //stay away from the right edge
  } //if
  else vCameraPos.x = m_vWorldSize.x/2.0f; //center horizontally.
  
  if(m_vWorldSize.y > m_nWinHeight){ //world higher than screen
    vCameraPos.y = max(vCameraPos.y, m_nWinHeight/2.0f);  //stay away from the bottom edge
    vCameraPos.y = min(vCameraPos.y, m_vWorldSize.y - m_nWinHeight/2.0f); //stay away from the top edge
  } //if
  else vCameraPos.y = m_vWorldSize.y/2.0f; //center vertically

  m_pRenderer->SetCameraPos(vCameraPos); //camera to player
} //FollowCamera

/// Handle keyboard input, move the game objects and render 
/// them in their new positions and orientations. Notify the 
/// audio player at the start of each frame so that it can 
/// prevent multiple copies of a sound from starting on the
/// same frame. Notify the timer of the start and end of the
/// frame so that it can calculate frame time. 

void CGame::ProcessFrame(){

 	if (currentGameplayState == STATE_SCREEN_MAINMENU)
	{
		m_pKeyboard->GetState(); //get current keyboard state 
		
		//Tick every update
		m_pStepTimer->Tick([&]() {
			});

		if (m_pKeyboard->TriggerDown('1'))
		{
			BeginGame('E');
		}
		else if (m_pKeyboard->TriggerDown('2'))
		{
			BeginGame('H');
		}
		else if (m_pKeyboard->TriggerDown('3'))
		{
			BeginGame('M');
		}
	}
	else if (currentGameplayState == STATE_SCREEN_ENDLEVEL)
	{
		//m_pAudio->stop();
		m_pMusicManager->stop();
		m_pAudio->play(END_SOUND, 0.5f);
		ScoreboardHandler(); //Separate keyboard handler for the scoreboard

		//Tick every update
		m_pStepTimer->Tick([&]() {
			m_pUI->Update();
			});

		if (m_pPOINTER->GetTimeScoreSubmitted() != 0.0f)
			if (m_pStepTimer->GetTotalSeconds() > (m_pPOINTER->GetTimeScoreSubmitted() + 5.0f))
			{
				currentGameplayState = STATE_SCREEN_MAINMENU;
				BackgroundColor = { 0.2f, 0.2f, 0.2f, 0.2f };
				m_pAudio->stop();
			}
	}
	else
	{
		if (!m_pPlayer->IsDead())
		{
			KeyboardHandler(); //handle keyboard input
			ControllerHandler(); //handle controller input

		}
		else
		{
			//Stop music
			m_pMusicManager->stop();

			m_pKeyboard->GetState(); //get current keyboard state 

			//Check if the current timer exceeds the player's time of Death plus an additional 6 seconds
			if (m_pStepTimer->GetTotalSeconds() > (m_pPlayer->GetTimeOfDeath() + 2.5f) && m_pKeyboard->TriggerDown())
			{
				EventOnPlayerDeath();
			}

		}

		m_pAudio->BeginFrame(); //notify audio player that frame has begun
		m_pMusicManager->BeginFrame();

		m_pStepTimer->Tick([&]() {
			m_pUI->Update();
			m_pHPUI->Update();
			m_pNUMA->Update();
			m_pNUMB->Update();
			m_pNUMC->Update();
			m_pNUMD->Update();
			m_pVISION->Update();
			m_pBATTERY->Update();
			m_pObjectManager->move(m_nCurLevelID, m_nCurRoomID); //move all objects
			FollowCamera(); //make camera follow player
			m_pParticleEngine->step(); //advance particle animation
			});

		//Listener for Level transfers on each frame
		if (m_pObjectManager->bLevelTransfer)
		{
			NextLevel();
		}

		//Switch the music if there is a change
		if (m_pObjectManager->bMusicChanged)
		{
			float	timeElasped = m_pStepTimer->GetTotalSeconds() - m_pObjectManager->GetTimeSinceMusicStateChanged();

			float newVolume = max(0.15f + (timeElasped * -0.1f), 0.009f);

			//Fade track to 0.0f
			if (m_pMusicManager->Fade(CurrentTrack, newVolume) == true)
			{
				CurrentTrack = m_pMusicManager->loop(m_pMapManager->GetSoundEnum(m_nCurLevelID, m_pObjectManager->GetMusicState()), 0.15f);

				//Reset Music Changed to false since we've changed music
				m_pObjectManager->bMusicChanged = false;
			}
		}
	}

  RenderFrame(); //render a frame of animation
} //ProcessFrame

/* 
============================================================================
								EVENTS
============================================================================ 
 */

void CGame::EventOnPlayerDeath()
{
	string file_map;
	//Reset to beginning of game
	m_nCurLevelID = 0;
	m_nCurRoomID = 0;

	//Reject any changes made so far in the level
	m_pObjectManager->DropGameState();

	//Delete everything in history
	m_pHistoryManager->FlushHistory();

	file_map = m_pMapManager->GetFilePath(m_nCurLevelID, m_nCurRoomID);

	m_pTileManager->LoadMap(&file_map[0]); //load map

	m_pAudio->stop();

	BeginGame(m_pPOINTER->getDif());
}

/*
============================================================================
								MENU SCREENS
============================================================================
*/


//Can only be processed in RenderFrame()
//Function for creating the game over screen
void CGame::UI_GameOver()
{
	//Get time elasped since player's death
	float	timeElasped = m_pStepTimer->GetTotalSeconds() - m_pPlayer->GetTimeOfDeath();
	float	multiplier = 0.8f;

	//Interpolate screen to red
	CSpriteDesc2D screenSpace;

	screenSpace.m_nSpriteIndex = NVISION_SPRITE;
	screenSpace.m_f4Tint.x = min(timeElasped * multiplier, 0.8f); //Linear regression: y = Ax
	screenSpace.m_f4Tint.y = 0.0f;
	screenSpace.m_f4Tint.z = 0.0f;
	screenSpace.m_f4Tint.w = 0.0f;

	screenSpace.m_fAlpha = min(timeElasped * 1.25f, 0.7f); //Linear regression: y = Ax

	screenSpace.m_nCurrentFrame = 3;
	screenSpace.m_vPos = m_vWinCenter;

	m_pRenderer->Draw(screenSpace);

	/* Fade in You Died sprite */

	CSpriteDesc2D desc;

	desc.m_nSpriteIndex = UI_MSG_DEATH;
	desc.m_fAlpha = min(timeElasped * multiplier, 1.0f); //Linear regression: y = Ax
	desc.m_vPos = Vector2(m_vWinCenter.x, m_vWinCenter.y + 50);

	m_pRenderer->Draw(desc);

	/* Set Background to red*/
	/* NOTE: Replace this with screen overlay instead! */

	/* Fade out Map up to 0.3 so text is readable*/
	CSpriteDesc2D bgFade;
	bgFade.m_nSpriteIndex = m_pMapManager->GetImageEnum(m_nCurLevelID, m_nCurRoomID);

	//Inverse of y = Ax
	bgFade.m_fAlpha = min(timeElasped / multiplier, 0.3f);

	/* Fade out Map up to 0.3 so text is readable*/
	if (timeElasped > 1.5f)
	{
		string s = "Press any key to restart...";

		XMVECTORF32 TextCol = { 1.0f, 1.0f, 1.0f, 0.0f };
		TextCol.f[3] = min((timeElasped - 1.5f) * 0.4f, 1.0f); //Linear regression: y = Ax 

		//From CRenderer3D::DrawCenteredText
		RECT r = m_pRenderer->GetFontBoundingRect(s.c_str());
		float w = (float)r.right - r.left; //text width in pixels
		float h = (float)r.bottom - r.top; //text height in pixels
		XMFLOAT2 pos(m_vWinCenter.x - w / 2, (m_vWinCenter.y - h) + 50); //text position

		m_pRenderer->DrawScreenText(s.c_str(), pos, TextCol);
	}

}

// TODO: Main Menu

void CGame::UI_MainMenu()
{
	string s1 = "MAN OF STEAL";

	float w;
	float h;
	RECT r1;
	XMFLOAT2 pos;

	XMVECTORF32 TextCol = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMVECTORF32 TextCol2 = { 1.0f, 1.0f, 1.0f, 0.8f };
	XMVECTORF32 TextCol3 = { 1.0f, 1.0f, 1.0f, 0.7f };

	CSpriteDesc2D d;
	d.m_nSpriteIndex = UI_SCREEN_TITLE;
	d.m_vPos = m_vWinCenter;
	d.m_nCurrentFrame = int(m_pStepTimer->GetTotalSeconds() * 8.0f) % 3;
	m_pRenderer->Draw(d);

	//From CRenderer3D::DrawCenteredText
	r1 = m_pRenderer->GetFontBoundingRect(s1.c_str());
	w = (float)r1.right - r1.left; //text width in pixels
	h = (float)r1.bottom - r1.top; //text height in pixels
	pos = XMFLOAT2(m_vWinCenter.x - w / 2, (m_vWinCenter.y - h)); //text position

	m_pRenderer->DrawScreenText(s1.c_str(), pos, TextCol);

	string s2 = "PRESS [1] for Easy,";

	//From CRenderer3D::DrawCenteredText
	r1 = m_pRenderer->GetFontBoundingRect(s2.c_str());
	w = (float)r1.right - r1.left; //text width in pixels
	h = (float)r1.bottom - r1.top; //text height in pixels
	pos = XMFLOAT2(m_vWinCenter.x - w / 2, (m_vWinCenter.y + h)); //text position

	m_pRenderer->DrawScreenText(s2.c_str(), pos, TextCol);

	string s4 = "PRESS [2] for Hard,";

	//From CRenderer3D::DrawCenteredText
	r1 = m_pRenderer->GetFontBoundingRect(s4.c_str());
	w = (float)r1.right - r1.left; //text width in pixels
	h = (float)r1.bottom - r1.top; //text height in pixels
	pos = XMFLOAT2(m_vWinCenter.x - w / 2, (m_vWinCenter.y + 2.25 * h)); //text position

	m_pRenderer->DrawScreenText(s4.c_str(), pos, TextCol);

	string s5 = "PRESS [3] for MAN OF STEAL";
	//From CRenderer3D::DrawCenteredText
	r1 = m_pRenderer->GetFontBoundingRect(s5.c_str());
	w = (float)r1.right - r1.left; //text width in pixels
	h = (float)r1.bottom - r1.top; //text height in pixels
	pos = XMFLOAT2(m_vWinCenter.x - w / 2, (m_vWinCenter.y + 4 * h)); //text position

	m_pRenderer->DrawScreenText(s5.c_str(), pos, TextCol);

	string s3 = "Press the [Tab] key while \nin the game to show controls.";

	m_pRenderer->DrawScreenText(s3.c_str(), Vector2(35.0f, 675.f), TextCol2);

}

void CGame::UI_EndLevel()
{
	float w;
	float h;
	string s1;
	XMFLOAT2 pos1;

	//Set color to white
	BackgroundColor = { 1.0f, 1.0f, 1.0f, 1.0f };


	if (m_pPOINTER->GetTimeScoreSubmitted() == 0.0f)
	{
		s1 = "Keys:\n[W] or [S]: Change letter.\n[A] or [D]: Move left or right.\n[Enter]: Submit score.";
		RECT r1 = m_pRenderer->GetFontBoundingRect(s1.c_str());
		w = (float)r1.right - r1.left; //text width in pixels
		h = (float)r1.bottom - r1.top; //text height in pixels
		pos1 = XMFLOAT2(m_vWinCenter.x - w / 2, (m_vWinCenter.y + h) + 100); //text position
	}
	else
	{
		s1 = "Score Submitted.\nReturning to Main Menu...";
		RECT r1 = m_pRenderer->GetFontBoundingRect(s1.c_str());
		w = (float)r1.right - r1.left; //text width in pixels
		h = (float)r1.bottom - r1.top; //text height in pixels
		pos1 = XMFLOAT2(m_vWinCenter.x - w / 2, (m_vWinCenter.y + h) + 175); //text position
	}

	XMVECTORF32 TextCol = { 0.0f, 0.0f, 0.0f, 1.0f };
	m_pRenderer->DrawScreenText(s1.c_str(), pos1, TextCol);
}