#pragma once

#include <list>

#include "Object.h"
#include "Player.h"
#include "Guard.h"
#include "UI.h"
#include "Map.h"
#include "Component.h"
#include "Common.h"
#include "Settings.h"
#include "TileManager.h"
#include "Gold.h"

struct GoldStateObject
{
	int ID;
	int amount;
};

// This is an individual state that gets created, and this object gets submitted to the HistoryManager. 

class CGameState :
	public CComponent,
	public CCommon,
	public CSettings {

public:
	CGameState();
	CGameState(CGameState*);
	CGameState(int curLevelID, int curRoomID);
	~CGameState();
	int GetLevelID() { return LevelID; };
	int GetRoomID() { return RoomID; };
	void SetLevelID(int lID) { LevelID = lID; };
	void SetRoomID(int rID) { RoomID = rID; };
	list<int*> GetGuardsKilled();
	list<GoldStateObject*> GetGoldCollected();
	list<GoldStateObject*> GetBatteryCollected();
	void AddGuardKilled(int GuardID);
	void AddGoldCollected(int id, int value);
	void AddBatteryCollected(int id, int value);
	bool IsObjEmpty() { return isEmpty; };
private:
	int ObjectID = -1;
	int LevelID = 0;
	int RoomID = 0;
	list<int* > ID_GuardsKilled;
	list<GoldStateObject*> ID_GoldCollected;
	list<GoldStateObject*> ID_BatteryCollected;
	bool isEmpty = true;
protected:
};