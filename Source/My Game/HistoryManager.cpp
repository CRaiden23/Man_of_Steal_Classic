#include "HistoryManager.h"

CHistoryManager::~CHistoryManager()
{
	for (auto const& p : gameHistory)
		delete p;
}

void CHistoryManager::WriteHistoryToDisk()
{
}

void CHistoryManager::LoadHistoryFromDisk()
{
}

void CHistoryManager::SubmitGameState(CGameState* gamestate)
{
	//Add to history
	if (!(gamestate->IsObjEmpty()))
		gameHistory.push_back(gamestate);
}

CGameState* CHistoryManager::RetrieveSingletonGameState(int ObjectID, int RoomID, int LevelID)
{
	if (ObjectID != -1)
		return gameHistory.at(ObjectID);
	else if (RoomID || LevelID)
	{
		for (auto const& p : gameHistory)
			if (p->GetLevelID() == LevelID && p->GetRoomID() == RoomID)
				return p;
	}

	//Return nullptr since there's no Gamestate that meets this critera
	return nullptr;
}

vector<CGameState*> CHistoryManager::RetrieveAllGameStatesByCriteria(int LevelID, int RoomID)
{
	vector<CGameState*> gs;

	for (auto const& p : gameHistory)
		if (p->GetLevelID() == LevelID && p->GetRoomID() == RoomID)
			gs.push_back(p);

	return gs;
}

void CHistoryManager::FlushHistory()
{
	for (auto const& p : gameHistory)
		delete p;

	gameHistory.clear();
}
