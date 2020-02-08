/// \file Guard.h
/// \brief Interface for the game object class CUI.

#pragma once

#include "Object.h"
#include <fstream>

struct infor
{
	char init[3];
	int score;
	char dif;
};

/// \brief The game object. 
///
/// CUI is the abstract representation of an object.

class CUI : public CObject {
	friend class CObjectManager; ///< The object manager needs access to manage objects, including the player.


private:
	//Vector2 m_vPos = Vector2::Zero; ///< Last position.
	Vector2 m_vVelocity = Vector2::Zero; ///< Velocity.
	float m_fFrameInterval = 0.1f; ///< Interval between frames.
	float m_fFrameTimer = 0.0f; ///< Last time the frame was changed.
	int lastHealth; //player's last health
	int score; // player's score
	int tiny; // variable used to caluclate score
	int big; // variable used to calculate score
	int vision; // whether the player is in a dark room or not
	int batteryLife; // battery life
	vector<infor> storage; //stores leaderboard from file
	char initials[3]; // player initials
	float timeScoreSubmitted = 0.0f; //Time passed since a score was submitted
	char difficulty; // difficulty of the level
public:
	CUI() { score = 0; }; ///< default Constructor
	CUI(eSpriteType t, const Vector2& p); ///< Constructor
	char getDif() { return difficulty; }; // returns difficulty of game
	void setDif(char set); // sets difficulty of the game
	void changePos(Vector2 &newPos); // change position of box UI
	Vector2 getPos() { return m_vPos; }; // returns position of objects
	void Update(); // updates all ui
	void addScore(int add); // adds to score
	void setScore(int temp); // sets score
	int getScore() { return score; }; //returns score
	void setVisionD(); // sets room to dark
	void setVisionL(); // sets room to not dark
	int getVision() { return vision; }; // returns value of whether it's dark or not
	void changeVision(); // switches between night vision and normal
	void incremB(); // increases battery life by 
	void decreaseB(); // decreases battery life
	void usingDec(); // when using night vision, it decreases by 5%
	void increaseB(); // increases battery life by 40%
	void setBattery(int temp); // sets battery life
	int getBattery() { return batteryLife; }; // returns battery life
	void getFile(); // reads in file and stores into storage vector
	vector<infor> getStorage() { return storage; }; // returns vector storage to display on screen later on
	void getInitials(char *temps); // a char array is passed and becomes the initials array we have in the class
	void changeInitials(int index, int direction); // allows the player to switch between certain chars
	void storeSort(); // stores initials and score value, and sorts from greatest to least score
	void writeFile(); // writes the score vector to a file

	void SetTimeScoreSubmitted(float time) { timeScoreSubmitted = time; };
	float GetTimeScoreSubmitted() { return timeScoreSubmitted; };

}; //CUI

/*

/// \file Guard.h
/// \brief Interface for the game object class CUI.

#pragma once

#include "Object.h"

/// \brief The game object. 
///
/// CUI is the abstract representation of an object.

class CUI : public CObject {
	friend class CObjectManager; ///< The object manager needs access to manage objects, including the player.

private:
	BoundingSphere m_Sphere; ///< Bounding sphere.
	BoundingBox m_Aabb; ///< Axially aligned bounding box.

	vector<Vector2> patrolPoints; // figure out how to assign patrol points

	int lastPatrolPoint = 0;

	//	Vector2 m_vPos = Vector2::Zero;
	Vector2 m_vOldPos = Vector2::Zero; ///< Last position.
	Vector2 m_vVelocity = Vector2::Zero; ///< Velocity.

	Vector2 m_vRadius = Vector2::Zero; ///< Half width and height of object sprite.

	float m_fGunTimer = 0.0f; ///< Gun fire timer.

	int guardHealth = 3;
	float guardSpeed = 75.0f;

	Vector2 playerSoundRadius = Vector2::Zero;
	float soundRadiusMod = 0.0f;

public:
	CUI(); ///< default Constructor
	CUI(const Vector2& p); ///< Constructor

	void move(const Vector2& dir);

	const Vector2& getNextPatrolPoint() { return patrolPoints.at(lastPatrolPoint + 1); }

	const Vector2& getGuardPos() { return m_vPos; }
	void setGuardPos(const Vector2& pos) { m_vPos = pos; }

	void setHealth(int newHealth) { guardHealth = newHealth; }
	int getHealth() { return guardHealth; }

	float getSpeed() { return guardSpeed; }


}; //CUI
*/