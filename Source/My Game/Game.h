/// \file Game.h
/// \brief Interface for the game class CGame.

#pragma once

#include "Component.h"
#include "Common.h"
#include "ObjectManager.h"
#include "Settings.h"

/// \brief The game class.

class CGame: 
  public CComponent, 
  public CSettings,
  public CCommon{ 

  private:
    void BeginGame(char difficulty); ///< Begin playing the game.
    void KeyboardHandler(); ///< The keyboard handler.
	void ScoreboardHandler();
    void ControllerHandler(); ///< The controller handler.
    void RenderFrame(); ///< Render an animation frame.
    void CreateObjects(); ///< Create game objects.
	void CreateGuards(int n);
	void CreateTraps(int n);
	void CreateGold(int n);
	void CreateBattery(int n);
	void SpawnGoldOnMap(int it, CGameState* &newGameState);
	void SpawnBatteryOnMap(int it, CGameState*& newGameState);
	void CreateUI();
    void FollowCamera(); ///< Make camera follow player character.
    void MoveRoom(); ///< Start next level.
	void NextLevel();
	void EventOnPlayerDeath(); ///< Handles player death
	void setDarkness();

	//Screens
	void UI_GameOver();
	void UI_MainMenu();
	void UI_EndLevel();
  public:
    ~CGame(); ///< Destructor.

    void Initialize(); ///< Initialize the game.
    void ProcessFrame(); ///< Process an animation frame.
    void Release(); ///< Release the renderer.
}; //CGame
