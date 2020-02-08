#include "GameState.h"

CGameState::CGameState()
{

}

CGameState::CGameState(CGameState* ptr)
{
	//Deep copy minus ObjectID
	this->ObjectID++;
	this->LevelID = ptr->LevelID;
	this->RoomID = ptr->RoomID;

	for (auto const& a : ptr->ID_GoldCollected)
	{
		this->AddGoldCollected(a->ID, a->amount);
	}

	for (auto const& b : ptr->ID_GuardsKilled)
	{
		this->AddGuardKilled(*b);
	}

	for (auto const& c : ptr->ID_BatteryCollected)
	{
		this->AddBatteryCollected(c->ID, c->amount);
	}

	//Since this is a deep copy from a pointer, this object is not empty
	this->isEmpty = false;
}

CGameState::CGameState(int curLevelID, int curRoomID)
{
	ObjectID++;
	LevelID = curLevelID;
	RoomID = curRoomID;
}

CGameState::~CGameState()
{
	for (auto const& p: ID_GoldCollected)
		delete p;

	ID_GoldCollected.clear();

	for (auto const& q : ID_GuardsKilled)
		delete q;

	ID_GuardsKilled.clear();

	for (auto const& r : ID_BatteryCollected)
		delete r;

}
list<int*> CGameState::GetGuardsKilled()
{
	return ID_GuardsKilled;
}

list<GoldStateObject*> CGameState::GetGoldCollected()
{
	return ID_GoldCollected;
}

list<GoldStateObject*> CGameState::GetBatteryCollected()
{
	return ID_BatteryCollected;
}

void CGameState::AddGuardKilled(int GuardID)
{
	int* newGID = new int;
	*newGID = GuardID;
	ID_GuardsKilled.push_back(newGID);
	isEmpty = false;
}

void CGameState::AddGoldCollected(int id, int value)
{
	struct GoldStateObject* GSO = new struct GoldStateObject;

	GSO->ID = id;
	GSO->amount = value;

	ID_GoldCollected.push_back(GSO);
	isEmpty = false;
}

void CGameState::AddBatteryCollected(int id, int value)
{
	struct GoldStateObject* BSO = new struct GoldStateObject;

	BSO->ID = id;
	BSO->amount = value;

	ID_BatteryCollected.push_back(BSO);
	isEmpty = false;
}
