/// \file HistoryManager.h
/// \brief Interface for the object manager CObjectManager.

#pragma once

#include <vector>

#include "Object.h"
#include "Component.h"
#include "Common.h"
#include "Settings.h"
#include "Gold.h"
#include "GameState.h"

using namespace std;

/// \brief History Manager
///
/// This class is create to keep track of the state of the game, from current menu to game stats and so forth. This is where all of the states are stored.
///
/// HOW IT WORKS:
/// This manager stores any interactions or changes that the player has done or hasn't done. This includes:
///			* Collecting Gold
///			* Killing Guards
///			* Triggering Events
///
/// When the player has done something meaningful, then the game must submit that change into the History Manager or it will not get saved!
/// Additionally, it has the ability to write these changes to the disk.

class CHistoryManager :
	public CComponent,
	public CCommon,
	public CSettings {

	public:
//		CHistoryManager();
		~CHistoryManager();
		void WriteHistoryToDisk();
		void LoadHistoryFromDisk();

		//The main function
		void SubmitGameState(CGameState* gamestate);
		CGameState* RetrieveSingletonGameState(int ObjectID = -1, int RoomID = 0, int LevelID = 0);
		vector<CGameState*> RetrieveAllGameStatesByCriteria(int LevelID, int RoomID);
		void FlushHistory();
	protected:
	private:
		vector<CGameState*> gameHistory;

};