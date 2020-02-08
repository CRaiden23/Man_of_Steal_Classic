/// \file ObjectManager.cpp
/// \brief Code for the the object manager class CObjectManager.

#include "ObjectManager.h"
#include "ComponentIncludes.h"
#include "ParticleEngine.h"
#include "helpers.h"
#include "ElectricFloor.h"
#include "Utilities.h"

/// Create the tile manager.

CObjectManager::CObjectManager(){

} //constructor

/// Destruct all of the objects in the object list.

CObjectManager::~CObjectManager(){
  for(auto const& p: m_stdObjectList) //for each object
    delete p; //delete object
} //destructor

/// Create an object and put a pointer to it on the object list.
/// \param t Sprite type.
/// \param v Initial position..
/// \return Pointer to the object created.

CObject* CObjectManager::create(eSpriteType t, const Vector2& v){
	CObject* p = nullptr;

	switch (t) {
	case PLAYER_SPRITE:
		p = new CPlayer(v); break;
	case ARMEDGUARD_SPRITE:
		p = new CAGuard(v); break;
	case GUARD_SPRITE:
		p = new CGuard(v); break; // do we wanna keep count of guards in here?
	case BOSS_GRENADIER_SPRITE:
		p = new CBossGrenadier(v); break;	

	case CAMERA_SPRITE:
		p = new CCamera(v); break;

	case TRAP_PITFALL_SPRITE:
		p = new CTrapPitfall(v); break;
	case TRAP_ELECTRIC_SPRITE:
		p = new CTrapElectric(v); break;

	case MOVEUI_SPRITE:
		p = new CUI(MOVEUI_SPRITE, v); break;
	case BOXUI_SPRITE:
		p = new CUI(BOXUI_SPRITE, v); break;
	case HPUI_SPRITE:
		p = new CUI(HPUI_SPRITE, v); break;
	case NUMA_SPRITE:
		p = new CUI(NUMA_SPRITE, v); break;
	case NUMB_SPRITE:
		p = new CUI(NUMB_SPRITE, v); break;
	case NUMC_SPRITE:
		p = new CUI(NUMC_SPRITE, v); break;
	case NUMD_SPRITE:
		p = new CUI(NUMD_SPRITE, v); break;
	case NVISION_SPRITE:
		p = new CUI(NVISION_SPRITE, v); break;
	case BATTERY_SPRITE:
		p = new CUI(BATTERY_SPRITE, v); break;
	case SCOREUI_SPRITE:
		p = new CUI(SCOREUI_SPRITE, v); break;
	case POINTER_SPRITE:
		p = new CUI(POINTER_SPRITE, v); break;

	case GOLD_SPRITE:
		p = new CGold(v); break;
	case BATTERYP_SPRITE:
		p = new CBattery(v); break;

	default:
		p = new CObject(t, v); break;
	}
  m_stdObjectList.push_back(p); 
  return p;
} //create

CObject* CObjectManager::create(eSpriteType t, CObject* pObj, int rarity) {
	CObject* p = nullptr;

	switch (t)
	{
		case GOLD_SPRITE: p = new CGold(pObj->m_vPos); break;
		case BATTERYP_SPRITE: p = new CBattery(pObj->m_vPos); break;
		default: p = new CObject(t, pObj->GetPos()); break;
	} //switch

	if (rarity != 0)
		p->m_nSpriteIndex = rarity;

	m_stdObjectList.push_back(p);
	return p;
} //Create


void CObjectManager::createItemQuick(eSpriteType t, const Vector2& v, int rarity) {
	CObject* p = nullptr;

	switch (t)
	{
		case GOLD_SPRITE: p = new CGold(v); break;
		case BATTERYP_SPRITE: p = new CBattery(v); break;
		default: p = new CObject(t, v); break;
	} //switch

	if (rarity != 0)
		p->m_nSpriteIndex = rarity;

	m_stdObjectList.push_back(p);
} //Create

/// Delete all of the objects managed by the object manager. 
/// This involves deleting all of the CObject instances pointed
/// to by the object list, then clearing the object list itself.

void CObjectManager::clear(){
	for (auto const& p : m_stdObjectList) //for each object
		delete p; //delete object


  m_stdObjectList.clear(); //clear the object list

} //clear

void CObjectManager::CreateGameState()
{
	//Allocate memory for game state
	if (!(bBlockCreationOfGS))
	{
		tempGS = new CGameState(m_nCurLevelID, m_nCurRoomID);

		//Block additional attempts to create a gamestate until a new one is submitted
		bBlockCreationOfGS = true;
	}
	else //We have an existing gamestate, continue using it but re-assign variables
	{
		tempGS->SetLevelID(m_nCurLevelID);
		tempGS->SetRoomID(m_nCurRoomID);
	}
}

vector<CGuard*> CObjectManager::findAllGuards()
{
	vector<CGuard*> guardList;

	for (auto const& p : m_stdObjectList) //for each object
		if(p->m_nSpriteIndex == GUARD_SPRITE || p->m_nSpriteIndex == ARMEDGUARD_SPRITE)
			guardList.push_back(static_cast<CGuard*>(p));

	return guardList;
}

// Runs on every tick determining if the player has alerted a guard
bool CObjectManager::IsAGuardAlerted()
{
	for (auto const& p : m_pObjectManager->findAllGuards())
		if (static_cast<CGuard*>(p)->getState() == ALERT_STATE)
		{
			//This will always be set until the player gets spotted
			timeSinceGuardsAlerted = m_pStepTimer->GetTotalSeconds();
			return true;
		}

	return false;
}

// Runs on every tick determining if the player has alerted the boss
bool CObjectManager::IsBossAlerted()
{
	for (auto const& p : m_stdObjectList)
	{
		if (p->m_nSpriteIndex == BOSS_GRENADIER_SPRITE)	//Make sure it's not a false positive
		{
			if (static_cast<CBossGrenadier*>(p)->GetIsActive())
			{
				return true;
			}

		}
	}

	return false;
}

void CObjectManager::ActiveElectricTraps()
{
	for (auto const& p : m_stdObjectList)
		if (static_cast<CTrapElectric*>(p))
			static_cast<CTrapElectric*>(p)->SetKillState(true);
}


void CObjectManager::DeactiveElectricTraps()
{
	for (auto const& p : m_stdObjectList)
		if (static_cast<CTrapElectric*>(p))
			static_cast<CTrapElectric*>(p)->SetKillState(false);
}

void CObjectManager::ManageGameplayState(enum eGameplayState State)
{
	//Block if there is no change
	if (currentGameplayState != State)
	{
		currentGameplayState = State;

		switch (State)
		{
			case STATE_GAME_BOSS:
				ManageMusicState(MUS_BOSS);
				break;
			case STATE_GAME_ALERT:
			case STATE_GAME_CAUTION:
				ManageMusicState(MUS_ALERT);
				break;
			case STATE_GAME_GREEN:
			default:
				ManageMusicState(MUS_AMBIENT);
				break;
		}

	}

}

void CObjectManager::SubmitStateToHistory()
{
	//Don't do anything if the object is empty
	if (!(tempGS->IsObjEmpty()) && bBlockCreationOfGS)
	{
		CGameState* newGameState = new CGameState(tempGS);

		m_pHistoryManager->SubmitGameState(newGameState);

		delete tempGS;

		//Allow creation of another gamestate
		bBlockCreationOfGS = false;
	}
}

void CObjectManager::DropGameState()
{
	delete tempGS;
	//Allow creation of another gamestate
	bBlockCreationOfGS = false;
}

void CObjectManager::ManageMusicState(eMusicState musState)
{
	//To prevent overlap, only change if the music state is different from the previous one.
	if (currentMusicState != musState)
	{
		currentMusicState = musState;
		bMusicChanged = true;
		timeMusicStateChanged = m_pStepTimer->GetTotalSeconds();
	}
}

/*		Old Draw method
/// Draw the tiled background and all of the objects.
void CObjectManager::draw(){
  //draw tiled background
  m_pTileManager->Draw(TILE_SPRITE);
  if(m_bDrawAABBs)
    m_pTileManager->DrawBoundingBoxes(GREENLINE_SPRITE);

  //draw objects
  for(auto const& p: m_stdObjectList) //for each object
    m_pRenderer->Draw(*(CSpriteDesc2D*)p);
} //draw
*/

/*		New Draw Method			*/
void CObjectManager::draw() {

	if (m_bDrawAABBs)
		m_pTileManager->DrawBoundingBoxes(GREENLINE_SPRITE);

	if (bDrawControlScreen)
	{
		CSpriteDesc2D d;
		d.m_fXScale = 1.0f;
		d.m_fYScale = 1.0f;
		d.m_fAlpha = .8f;
		d.m_nSpriteIndex = UI_TUT_CONTROLS;
		d.m_vPos = m_vWinCenter;
		d.m_vPos.y += 100;

		m_pRenderer->Draw(d);
	}


	//draw objects
	for (auto const& p : m_stdObjectList) //for each object
	{
		if (p->m_nSpriteIndex == GUARD_SPRITE)
		{
			m_pRenderer->DrawFOV(p->m_vPos, static_cast<CGuard*>(p)->m_fFacingAngle);
		}
		else if (p->m_nSpriteIndex == CAMERA_SPRITE)
		{
			m_pRenderer->DrawFOV(p->m_vPos, p->m_fRoll);
		}
		else if (p->m_nSpriteIndex == ARMEDGUARD_SPRITE)
		{
			m_pRenderer->DrawFOV(p->m_vPos, static_cast<CAGuard*>(p)->m_fFacingAngle);
		}

		if (p->m_nSpriteIndex == TRAP_PITFALL_SPRITE || 
			p->m_nSpriteIndex == TRAP_ELECTRIC_SPRITE
		)
		{
			CSpriteDesc2D temp;
			temp = *(CSpriteDesc2D*)p;
			temp.m_f4Tint = roomColor;
			m_pRenderer->Draw(temp);
		}
		else
		{
			m_pRenderer->Draw(*(CSpriteDesc2D*)p);
		}


		if (m_bDrawAABBs)
			m_pRenderer->DrawBoundingBox(p->GetBoundingBox());
	}
	
	if (m_nCurRoomID == 999)// this is used to print the scoreboard to the screen
	{
		vector<infor> storage;
		storage = m_pPOINTER->getStorage();
		Vector2 compare = m_pPOINTER->getPos();
		char temp[3];
		m_pPOINTER->getInitials(temp);

		if(compare.x < 0)
			m_pPOINTER->changePos(Vector2(390.0f, 210.0f));
		float xaxis = 410;
		string pInitials;
		for (int i = 0; i < 3; i++)// prints initials that the player can select
		{
			pInitials.push_back(temp[i]);
			m_pRenderer->DrawScreenText(pInitials.c_str(), Vector2(xaxis, 550.0f));
			pInitials.clear();
			xaxis += 110;
		}
		string s = "Score";
		int scoreP = m_pSCORE->getScore();
		char tempDif = m_pPOINTER->getDif();
		m_pRenderer->DrawScreenText(s.c_str(), Vector2(470.0f, 25.0f));
		s = "Your Score:";
		m_pRenderer->DrawScreenText(s.c_str(), Vector2(25.0f, 25.0f));
		s = "Difficulty:";
		m_pRenderer->DrawScreenText(s.c_str(), Vector2(25.0f, 145.0f));
		s = to_string(scoreP);
		m_pRenderer->DrawScreenText(s.c_str(), Vector2(25.0f, 65.0f));
		if (tempDif == 'E')
		{
			s = "Easy";
		}
		else if (tempDif == 'H')
		{
			s = "Hard";
		}
		else if (tempDif == 'M')
		{
			s = "MAN OF STEAL";
		}
		m_pRenderer->DrawScreenText(s.c_str(), Vector2(25.0f, 185.0f));
		string initials;
		float yaxis = 75;
		for (int i = 0; i < storage.size(); i++)// prints actual scoreboard
		{
			initials.push_back(storage[i].init[0]);
			m_pRenderer->DrawScreenText(initials.c_str(), Vector2(430.0f, yaxis));
			initials.clear();
			initials.push_back(storage[i].init[1]);
			m_pRenderer->DrawScreenText(initials.c_str(), Vector2(460.0f, yaxis));
			initials.clear();
			initials.push_back(storage[i].init[2]);
			m_pRenderer->DrawScreenText(initials.c_str(), Vector2(490.0f, yaxis));
			initials.clear();
			initials = to_string(storage[i].score);
			m_pRenderer->DrawScreenText(initials.c_str(), Vector2(550.0f, yaxis));
			initials.clear();
			initials.push_back(storage[i].dif);
			m_pRenderer->DrawScreenText(initials.c_str(), Vector2(700.0f, yaxis));
			initials.clear();
			yaxis += 45;
		}
	}
		
} //draw

/*		Separate Draw method for rooms		*/
void CObjectManager::drawRoom(eSpriteType t) {

	//Background
	if (t != NUM_SPRITES)
	{
		CSpriteDesc2D desc;
		desc.m_nSpriteIndex = t;
		desc.m_vPos = m_vWinCenter;
		desc.m_f4Tint = roomColor; //Use variable from Common class

		if (m_pPlayer->IsDead())
			desc.m_fAlpha = min((m_pStepTimer->GetTotalSeconds() - m_pPlayer->GetTimeOfDeath()) / 0.8f, 0.2f);
		else
			desc.m_fAlpha = 1.0f;

		m_pRenderer->Draw(desc);
	}

} //draw

//Event that tells the Game to move to another level
void CObjectManager::IssueLevelTransferCall(Vector2 oldVectPos, int col_opcode)
{
	oldPlayerPos = oldVectPos;
	bLevelTransfer = true;
	iCollisionCode = col_opcode;
}

/// Move all of the objects and perform 
/// broad phase collision detection and response.

void CObjectManager::move(int lID, int rID){

  for(auto const& p: m_stdObjectList){ //for each object

	  if    (p->m_nSpriteIndex == MOVEUI_SPRITE 
		  || p->m_nSpriteIndex == HPUI_SPRITE 
		  || p->m_nSpriteIndex == GOLD_SPRITE 
		  || p->m_nSpriteIndex == BOXUI_SPRITE
		  || p->m_nSpriteIndex == NUMA_SPRITE
		  || p->m_nSpriteIndex == NUMB_SPRITE
		  || p->m_nSpriteIndex == NUMC_SPRITE
		  || p->m_nSpriteIndex == NUMD_SPRITE
		  || p->m_nSpriteIndex == NVISION_SPRITE
		  || p->m_nSpriteIndex == BATTERY_SPRITE
		  || p->m_nSpriteIndex == SCOREUI_SPRITE
		  || p->m_nSpriteIndex == BATTERYP_SPRITE
		  || p->m_nSpriteIndex == POINTER_SPRITE) // these are things that should not move
		  continue;

	  if (p->getHealth() <= 0)
	  {
		  CGold* pGold;

		  switch (p->m_nSpriteIndex) {
		  case GUARD_SPRITE:
			  pGold = (CGold*)create(GOLD_SPRITE, p); // drop a piece of gold upon death
			  pGold->m_nCurrentFrame = m_pRandom->randn(0, 4);
			  break;
		  case ARMEDGUARD_SPRITE:
			  pGold = (CGold*)create(GOLD_SPRITE, p); // drop a piece of gold upon death
			  pGold->m_nCurrentFrame = m_pRandom->randn(4, 7);
			  break;
		  case BOSS_GRENADIER_SPRITE:
			  for (int i = 0; i < 8; i++)
			  {
				  pGold = (CGold*)create(GOLD_SPRITE, RandVector(p->GetPos(), 55.0f)); // drop a piece of gold upon death
				  pGold->m_nCurrentFrame = m_pRandom->randn(3, 6);
			  }

			  //Spawn 1 rare gold piece
			  pGold = (CGold*)create(GOLD_SPRITE, p); // drop a piece of gold upon death
			  pGold->m_nCurrentFrame = m_pRandom->randn(7, 8);
			  break;
		  }
		  p->kill();
	  }

	  if (p->m_nSpriteIndex == PUNCH_SPRITE)
		  if (p->lifetime + .15f < m_pStepTimer->GetTotalSeconds()) // 'bullets' disappear after so long
		  {
			  p->killFX();
		  }

	  if (p->m_nSpriteIndex == EXPLOSION_SPRITE || p->m_nSpriteIndex == EXPLOSION_FAKE_SPRITE)
		  if (p->lifetime + .65f < m_pStepTimer->GetTotalSeconds())
			  p->killFX();

		const Vector2 oldpos = p->m_vPos; //its old position
		
		switch (p->m_nSpriteIndex) // special move functions
		{
		case GUARD_SPRITE:
				static_cast<CGuard*>(p)->move();		break;
		case ARMEDGUARD_SPRITE:
				static_cast<CAGuard*>(p)->move();		break;
		case TRAP_PITFALL_SPRITE:
			static_cast<CTrapPitfall*>(p)->move();		break;
		case TRAP_ELECTRIC_SPRITE:
			static_cast<CTrapElectric*>(p)->move();		break;
		case CAMERA_SPRITE:
			static_cast<CCamera*>(p)->move();		break;
		case BOSS_GRENADIER_SPRITE:
				static_cast<CBossGrenadier*>(p)->move();		break;
		default: // anything else not special
			p->move();
			break;
		}
		

    
    //object to wall collision detection and response using
    //bounding spheres for the objects and AABBs for the walls.
	 int collisioncode = m_pTileManager->CollideWithSpecial(p->GetBoundingSphere());

	//anything that's not -1 means we hit the special wall
	if(collisioncode != -1)
		switch (p->m_nSpriteIndex)
		{
			case PLAYER_SPRITE:
			{
					p->CollisionResponse();

				//Tell the object manager we want to transfer level
				IssueLevelTransferCall(p->m_vPos, collisioncode);
			}
			break;

			default:
				break;
		}

	vector<BoundingBox> walls;

	if (m_pTileManager->CollideWithWall(p->GetBoundingBox(), walls))
		switch (p->m_nSpriteIndex) {
		case PLAYER_SPRITE:		
			for (auto const& b : walls)
				p->CollisionResponse(b);
			break;

		case ARMEDGUARD_SPRITE:
		case BOSS_GRENADIER_SPRITE:
		case GUARD_SPRITE:
			for (auto const& b : walls)
				static_cast<CGuard*>(p)->CollisionResponse(b);	
			break;

		case PUNCH_SPRITE:
			m_pAudio->play(PUNCHWALL_SOUND);
			p->killFX();
			break;

		case BULLET2_SPRITE:
			p->killFX();
			break;

		case BULLET_GRENADE_SPRITE:
			CreateExplosion(p->GetPos(), EXPLOSION_SPRITE);
			p->killFX();
			break;

		case TRAP_PITFALL_SPRITE:	
		case TRAP_ELECTRIC_SPRITE:
			break;
		default:
			for (auto const& b : walls)
				p->CollisionResponse(b);	
			break;
		} //switch
  } //for

  //now do object-object collision detection and response and
  //remove any dead objects from the object list.

  BroadPhase(); //broad phase collision detection and response
  CullDeadObjects(); //remove dead objects from object list


  //Check if this room is a boss room before doing any checks
  if (m_pMapManager->GetMap(m_nCurLevelID, m_nCurRoomID)->isBossRoom)
  {
	  if (IsBossAlerted())
	  {
		  ManageGameplayState(STATE_GAME_BOSS);
		  ActiveElectricTraps();
	  }
	  else
	  {
		  ManageGameplayState(STATE_GAME_GREEN);
		  DeactiveElectricTraps();
	  }
  }
  else
  {
	  //Manage the current state of the game
	  if (IsAGuardAlerted())

		  ManageGameplayState(STATE_GAME_ALERT);
	  else
		  //Hold for 5 seconds to prevent the game from constantly trying to start the track
		  if (m_pStepTimer->GetTotalSeconds() > timeSinceGuardsAlerted + 5.0f)
			  ManageGameplayState(STATE_GAME_GREEN);
  }
} //move

/// Create a bullet object and a flash particle effect.
/// It is assumed that the object is round and that the bullet
/// appears at the edge of the object in the direction
/// that it is facing and continues moving in that direction.
/// \param pObj Pointer to an object.
/// \param bullet Sprite type of bullet.

void CObjectManager::Punch(CObject* pObj, eSpriteType bullet){
	if (m_pStepTimer->GetTotalSeconds() > m_pPlayer->m_fTimeofLastAtk + m_pPlayer->m_fAtkCooldown)
	{
		m_pPlayer->m_fTimeofLastAtk = m_pStepTimer->GetTotalSeconds();

		m_pPlayer->attack = true;
		m_pAudio->play(PUNCH_SOUND, 0.5f);

		Vector2 view = pObj->GetVelocity();
		if (view.x == 0.0f && view.y == 0.0f)
			switch (pObj->facingDir)
			{
			case fBACK:
				view = Vector2(0, 1);
				break;
			case fFRONT:
				view = Vector2(0, -1);
				break;
			case fLEFT:
				view = Vector2(-1, 0);
				break;
			case fRIGHT:
				view = Vector2(1, 0);
				break;
			}

		Vector2 pos = pObj->GetPos() + 1.5f * m_pRenderer->GetWidth(pObj->m_nSpriteIndex) * view;

		//create punch object

		CObject* pPunch = m_pObjectManager->create(bullet, pos); //create bullet

		pPunch->lifetime = m_pStepTimer->GetTotalSeconds();
	}
} //FireGun

void CObjectManager::FireGun(CObject* pObj, eSpriteType bullet) {
	m_pAudio->play(GUN_SOUND);

	const Vector2 view(-sin(static_cast<CGuard*>(pObj)->getFacingAngle()), cos(static_cast<CGuard*>(pObj)->getFacingAngle()));
	Vector2 pos = pObj->GetPos() + 0.5f * m_pRenderer->GetWidth(pObj->m_nSpriteIndex) * view;

	//create bullet object

	CObject* pBullet = m_pObjectManager->create(bullet, pos); //create bullet

	const Vector2 norm(view.y, -view.x); //normal to direction
	const float m = 2.0f * m_pRandom->randf() - 1.0f;
	const Vector2 deflection = 0.01f * m * norm;

	pBullet->SetSpeed(500.0f);
	pBullet->SetVelocity(pObj->GetVelocity() + 300.0f * (view + deflection));
	pBullet->SetOrientation(m_pPlayer->GetOrientation());
} //FireGun

// Copy of FireGun, except fires slower bullets that explode
void CObjectManager::FireGrenadeLauncher(CObject* pObj, eSpriteType bullet) {
	m_pAudio->play(GUN_SOUND);

	const Vector2 view(-sin(static_cast<CGuard*>(pObj)->getFacingAngle()), cos(static_cast<CGuard*>(pObj)->getFacingAngle()));
	Vector2 pos = pObj->GetPos() + 0.5f * m_pRenderer->GetWidth(pObj->m_nSpriteIndex) * view;

	//create bullet object

	CObject* pBullet = m_pObjectManager->create(bullet, pos); //create bullet

	const Vector2 norm(view.y, -view.x); //normal to direction
	const float m = 2.0f * m_pRandom->randf() - 1.0f;
	const Vector2 deflection = 0.01f * m * norm;

	pBullet->m_fFrameInterval = 0.2f;
	pBullet->SetSpeed(250.0f);
	pBullet->SetVelocity(pObj->GetVelocity() + 300.0f * (view + deflection));
	pBullet->SetOrientation(m_pPlayer->GetOrientation());
} //FireGrenadeLauncher

// Creates an explosion at an area. Does not move.
// Not a particle since we need to check if the player or other objects were caught in the blast.
void CObjectManager::CreateExplosion(Vector2 pos, eSpriteType explosion) {
	m_pAudio->play(FX_EXPLODE_SOUND);

	CObject* pExplosion = m_pObjectManager->create(explosion, pos); //create explosion

	pExplosion->m_fFrameInterval = 0.1f;
	pExplosion->lifetime = m_pStepTimer->GetTotalSeconds();
} //FireGrenadeLauncher

/// This is a "bring out yer dead" Monty Python type of thing.
/// Iterate through the objects and check whether their "is dead"
/// flag has been set. If so, then delete its pointer from
/// the object list and destruct the object.

void CObjectManager::CullDeadObjects() {
	for (auto i = m_stdObjectList.begin(); i != m_stdObjectList.end();) {
		if ((*i)->IsDead()) { //"He's dead, Dave." --- Holly, Red Dwarf
			if ((*i)->m_nSpriteIndex == PLAYER_SPRITE)
			{
				(*i)->m_vPos = Vector2::Zero;
			}
			else if ((*i)->m_nSpriteIndex == GUARD_SPRITE || (*i)->m_nSpriteIndex == ARMEDGUARD_SPRITE || (*i)->m_nSpriteIndex == BOSS_GRENADIER_SPRITE)
			{
				tempGS->AddGuardKilled((*i)->GetUniqueID());
				delete* i;
			}
			else
			{
				delete* i; //delete object
			}

			i = m_stdObjectList.erase(i); //remove from object list and advance to next object
    } //if
      
    else ++i; //advance to next object
  } //for
} //CullDeadObjects

/// Perform collision detection and response for all pairs
/// of objects in the object list, making sure that each
/// pair is processed only once.

void CObjectManager::BroadPhase(){
  for(auto i=m_stdObjectList.begin(); i!=m_stdObjectList.end(); i++){
    for(auto j=next(i); j!=m_stdObjectList.end(); j++)
      NarrowPhase(*i, *j);
  } //for
} //BroadPhase

/// Perform collision detection and response for a pair of objects.
/// We are talking about bullets hitting the player and the
/// turrets here. When a collision is detected the response
/// is to delete the bullet (i.e. mark its "is dead" flag)
/// play a particle effect at the point of contact, and play one 
/// sound for the player and another for the turrets.
/// \param p0 Pointer to the first object.
/// \param p1 Pointer to the second object.

void CObjectManager::NarrowPhase(CObject* p0, CObject* p1) {
	eSpriteType t0 = (eSpriteType)p0->m_nSpriteIndex;
	eSpriteType t1 = (eSpriteType)p1->m_nSpriteIndex;

	if (p0->m_Aabb.Intersects(p1->m_Aabb)) { //bounding spheres intersect
		if (t0 == GUARD_SPRITE && t1 == PLAYER_SPRITE) { //guard hits player
			p0->CollisionResponse();
			p1->takeDamage(p0->m_vPos, 1);
			static_cast<CGuard*>(p0)->takeDamage(p1->m_vPos, 0);// this should bounce the enemy back
			printf("Player: 'OUCH!'\n");
		} //if

		else if (t0 == PLAYER_SPRITE && t1 == GUARD_SPRITE) { //player touches guard
			p1->CollisionResponse();
			p0->takeDamage(p1->m_vPos, 1);
			static_cast<CGuard*>(p1)->takeDamage(p0->m_vPos, 0);// this should bounce the enemy back
			printf("Player: 'OUCH!'\n");
		} //if

		if (t0 == BOSS_GRENADIER_SPRITE && t1 == PLAYER_SPRITE) { //boss hits player
			p0->CollisionResponse();
			p1->takeDamage(p0->m_vPos * 1.5f, 1);
			static_cast<CGuard*>(p0)->takeDamage(p1->m_vPos, 0);// this should bounce the enemy back
			printf("Player: 'OUCH!'\n");
		} //if

		else if (t0 == PLAYER_SPRITE && t1 == BOSS_GRENADIER_SPRITE) { //player touches boss
			p1->CollisionResponse();
			p0->takeDamage(p1->m_vPos * 1.5f, 1);
			static_cast<CGuard*>(p1)->takeDamage(p0->m_vPos, 0); // this should bounce the enemy back
			printf("Player: 'OUCH!'\n");
		} //if

		else if ( (t0 == PLAYER_SPRITE && t1 == TRAP_PITFALL_SPRITE) ||
				  (t0 == GUARD_SPRITE && t1 == TRAP_PITFALL_SPRITE) ||
				  (t0 == ARMEDGUARD_SPRITE && t1 == TRAP_PITFALL_SPRITE) ||
				  (t0 == BOSS_GRENADIER_SPRITE && t1 == TRAP_PITFALL_SPRITE) ||
				  (t0 == GOLD_SPRITE && t1 == TRAP_PITFALL_SPRITE) ) { //player, guard, armed guard, or grenadier boss, or gold collides with pitfall
			static_cast<CTrapPitfall*>(p1)->CollisionResponse();
			bool bTriggered = static_cast<CTrapPitfall*>(p1)->GetTriggeredState();
			bool bKillState = static_cast<CTrapPitfall*>(p1)->GetKillState();
			if (bKillState)
			{
				//Kill the object, resolve in whatever class
				p0->kill();
			}

		} //if

		//Same as above, but in the opposite direction
		else if ((t0 == TRAP_PITFALL_SPRITE && t1 == PLAYER_SPRITE) ||
			(t0 == TRAP_PITFALL_SPRITE && t1 == GUARD_SPRITE) ||
			(t0 == TRAP_PITFALL_SPRITE && t1 == ARMEDGUARD_SPRITE) ||
			(t0 == TRAP_PITFALL_SPRITE && t1 == BOSS_GRENADIER_SPRITE) ||
			(t0 == TRAP_PITFALL_SPRITE && t1 == GOLD_SPRITE) ) { //pitfall collides with  player, guard, armed guard, or grenadier boss
			static_cast<CTrapPitfall*>(p0)->CollisionResponse();
			bool bTriggered = static_cast<CTrapPitfall*>(p0)->GetTriggeredState();
			bool bKillState = static_cast<CTrapPitfall*>(p0)->GetKillState();
			if (bKillState)
			{
				//Kill the object, resolve in whatever class
				p1->kill();
			}

		} //if

		else if (t0 == PLAYER_SPRITE && t1 == TRAP_ELECTRIC_SPRITE) { //player collides with electric trap
			static_cast<CTrapElectric*>(p1)->CollisionResponse();
			bool bKillState = static_cast<CTrapElectric*>(p1)->GetKillState();
			if (bKillState)
			{
				//Push the player back and take a heart
				p0->takeDamage(p1->m_vPos, 1);
				printf("Player: 'BZZZZZZZRT!'\n");
			}

		} //if

		//Same as above, but in the opposite direction
		else if (t0 == TRAP_ELECTRIC_SPRITE && t1 == PLAYER_SPRITE) { //player collides with electric trap
			static_cast<CTrapElectric*>(p0)->CollisionResponse();
			bool bKillState = static_cast<CTrapElectric*>(p0)->GetKillState();
			if (bKillState)
			{
				//Push the player back and take a heart
				p1->takeDamage(p0->m_vPos, 1);
				printf("Player: 'BZZZZZZZRT!'\n");
			}

		} //if

		//Explosions
		else if ((t0 == PLAYER_SPRITE && t1 == EXPLOSION_SPRITE) ||
			(t0 == GUARD_SPRITE && t1 == EXPLOSION_SPRITE) ||
			(t0 == ARMEDGUARD_SPRITE && t1 == EXPLOSION_SPRITE) ||
			(t0 == BOSS_GRENADIER_SPRITE && t1 == EXPLOSION_SPRITE)) { //player, guard, armed guard, or grenadier boss collides with explosion
			
			p0->takeDamage(p1->m_vPos * 1.5f, 1); // Really push them away

		} //if

		//Same as above but in the opposite direction
		else if ((t0 == EXPLOSION_SPRITE && t1 == PLAYER_SPRITE) ||
			(t0 == EXPLOSION_SPRITE && t1 == GUARD_SPRITE) ||
			(t0 == EXPLOSION_SPRITE && t1 == ARMEDGUARD_SPRITE) ||
			(t0 == EXPLOSION_SPRITE && t1 == BOSS_GRENADIER_SPRITE)
			) { //explosion collides with  player, guard, armed guard, or grenadier boss
			
			p1->takeDamage(p0->m_vPos * 1.5f, 1); // Really push them away

		} //if


		else if (t0 == GUARD_SPRITE && t1 == GUARD_SPRITE) { //Guard hits guard
			p1->CollisionResponse();
			static_cast<CGuard*>(p1)->takeDamage(p0->m_vPos * .25, 0);
		} //if

		else if ( (t0 == PUNCH_SPRITE && t1 == GUARD_SPRITE) || 
				  (t0 == PUNCH_SPRITE && t1 == ARMEDGUARD_SPRITE) ||
				  (t0 == PUNCH_SPRITE && t1 == BOSS_GRENADIER_SPRITE)
			) { //punch hits turret
			p1->CollisionResponse(p1->GetBoundingBox());
			static_cast<CGuard*>(p1)->takeDamage(p0->m_vPos,1);
			p0->killFX();
		} //if

		else if ( (t1 == PUNCH_SPRITE && t0 == GUARD_SPRITE) || 
				  (t1 == PUNCH_SPRITE && t0 == ARMEDGUARD_SPRITE) ||
				  (t1 == PUNCH_SPRITE && t0 == BOSS_GRENADIER_SPRITE)
			) { //turret hit by punch
			p0->CollisionResponse(p0->GetBoundingBox());
			static_cast<CGuard*>(p0)->takeDamage(p1->m_vPos,1);
			p1->killFX();
		} //else if

		else if (t0 == BULLET2_SPRITE && t1 == PLAYER_SPRITE) { //player hit by bullet
			p0->kill();
			p1->CollisionResponse();
			p1->takeDamage(p1->m_vPos, 1);
			printf("Player: 'OUCH!'\n");
		} //else if

		else if (t1 == BULLET2_SPRITE && t0 == PLAYER_SPRITE) { //bullet hits player
			p1->kill();
			p0->CollisionResponse();
			p0->takeDamage(p1->m_vPos, 1);
			printf("Player: 'OUCH!'\n");
		} //else if


		//Special case, destroy the bullet, but don't hurt the player. The explosion will do it instead.
		else if (t0 == BULLET_GRENADE_SPRITE && t1 == PLAYER_SPRITE) { //player hit by bullet
			CreateExplosion(p0->GetPos(), EXPLOSION_SPRITE);
			p0->killFX();
		} //else if

		//Same as above, but in the opposite direction
		else if (t1 == BULLET_GRENADE_SPRITE && t0 == PLAYER_SPRITE) { //bullet hits player
			CreateExplosion(p1->GetPos(), EXPLOSION_SPRITE);
			p1->killFX();
		} //else if

		else if (t1 == GOLD_SPRITE && t0 == PLAYER_SPRITE) { //player collects gold
			m_pAudio->play(MONEY_SOUND);
			m_pSCORE->addScore((p1->m_nCurrentFrame+1)*7);

			//Grab the ID and Gold collected and add it to the gamestate for submission
			//Must do a static_cast since we are most certain that we have a Gold class object
			tempGS->AddGoldCollected( static_cast<CGold*>(p1)->GetUniqueID(), (p1->m_nCurrentFrame + 1) * 5);
			p1->killFX();
		} //else if

		else if (t1 == BATTERYP_SPRITE && t0 == PLAYER_SPRITE) { //player collects battery
			m_pAudio->play(BATTERYP_SOUND);
			m_pBATTERY->increaseB();
			p1->killFX();
		} //else if
		else if (t1 == BOXUI_SPRITE && t0 == PLAYER_SPRITE)
		{
			float add = 854;
			float yaxis;
			if (m_pBUI->getPos().x == 96)
			{
				yaxis = m_pBUI->getPos().y;
				m_pBUI->changePos(Vector2(m_pBUI->getPos().x + add, yaxis));
				yaxis = m_pUI->getPos().y;
				m_pUI->changePos(Vector2(m_pUI->getPos().x + add, yaxis));
				yaxis = m_pHPUI->getPos().y;
				m_pHPUI->changePos(Vector2(m_pHPUI->getPos().x + add, yaxis));
				yaxis = m_pNUMA->getPos().y;
				m_pNUMA->changePos(Vector2(m_pNUMA->getPos().x + add, yaxis));
				yaxis = m_pNUMB->getPos().y;
				m_pNUMB->changePos(Vector2(m_pNUMB->getPos().x + add, yaxis));
				yaxis = m_pNUMC->getPos().y;
				m_pNUMC->changePos(Vector2(m_pNUMC->getPos().x + add, yaxis));
				yaxis = m_pNUMD->getPos().y;
				m_pNUMD->changePos(Vector2(m_pNUMD->getPos().x + add, yaxis));
				yaxis = m_pSCORE->getPos().y;
				m_pSCORE->changePos(Vector2(m_pSCORE->getPos().x + add, yaxis));
				yaxis = m_pSCOREUI->getPos().y;
				m_pSCOREUI->changePos(Vector2(m_pSCOREUI->getPos().x + add, yaxis));
				yaxis = m_pBATTERY->getPos().y;
				m_pBATTERY->changePos(Vector2(m_pBATTERY->getPos().x + add, yaxis));
			}
			else if (m_pBUI->getPos().x == 950)
			{
				yaxis = m_pBUI->getPos().y;
				m_pBUI->changePos(Vector2(m_pBUI->getPos().x - add, yaxis));
				yaxis = m_pUI->getPos().y;
				m_pUI->changePos(Vector2(m_pUI->getPos().x - add, yaxis));
				yaxis = m_pHPUI->getPos().y;
				m_pHPUI->changePos(Vector2(m_pHPUI->getPos().x - add, yaxis));
				yaxis = m_pNUMA->getPos().y;
				m_pNUMA->changePos(Vector2(m_pNUMA->getPos().x - add, yaxis));
				yaxis = m_pNUMB->getPos().y;
				m_pNUMB->changePos(Vector2(m_pNUMB->getPos().x - add, yaxis));
				yaxis = m_pNUMC->getPos().y;
				m_pNUMC->changePos(Vector2(m_pNUMC->getPos().x - add, yaxis));
				yaxis = m_pNUMD->getPos().y;
				m_pNUMD->changePos(Vector2(m_pNUMD->getPos().x - add, yaxis));
				yaxis = m_pSCORE->getPos().y;
				m_pSCORE->changePos(Vector2(m_pSCORE->getPos().x - add, yaxis));
				yaxis = m_pSCOREUI->getPos().y;
				m_pSCOREUI->changePos(Vector2(m_pSCOREUI->getPos().x - add, yaxis));
				yaxis = m_pBATTERY->getPos().y;
				m_pBATTERY->changePos(Vector2(m_pBATTERY->getPos().x - add, yaxis));
			}
		}
	} //if
} //NarrowPhase
