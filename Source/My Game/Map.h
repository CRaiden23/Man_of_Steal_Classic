#pragma once

//Loads and stores information about each map
//Can be retrieved for other functions to do as they wish
#include <vector>

#include "tinyxml2.h"
#include "Abort.h"

#include "GameDefines.h"
#include "SpriteRenderer.h"
#include "Common.h"


//
// NOTE: To transfer levels, every map needs to abide with these rules:
//
//					  NORTH
//
//				 111111111111111
//				 
//			4	 					2
//		W	4	 					2   E
//		E	4	 					2   A
//		S	4	 					2   S
//		T	4	 					2   T
//			4	 					2
//				
//				 3333333333333333
//
//					  SOUTH
//		
//
//	1 = NORTH
//	2 = EAST
//  3 = SOUTH
//  4 = WEST
//
//  Level transfer operation: if the player collides with these special characters, then the level will change, 
//			and the player will spawn on the other side of the level transfer barrier.
//
//  Here's the following possible operations:
//  1 -> 3
//	2 -> 4
//  3 -> 1
//	4 -> 2
//
// So the level transfer would look something like this
//
//						   NORTH
//
//					  333333333333333
//					 -----------------
//					  111111111111111
//				 
//			2 | 4	 					   2 | 4
//		W	2 | 4	 					   2 | 4   E
//		E	2 | 4	 					   2 | 4   A
//		S	2 | 4	 					   2 | 4   S
//		T	2 | 4	 					   2 | 4   T
//			2 | 4	 					   2 | 4
//				
//					  333333333333333
//					 -----------------
//					  111111111111111
//
//						   SOUTH
//
//  Further explanation below
//

//  The map struct has the following values:
//
//  roomID:			The ID of the room
//  comment:		Special comments about the room, not used otherwise.
//
//	N, E, S, W:		These are the roomIDs of the room that the level transfer will go to. 
//					Not to be confused with maps 1, 2, 3, 4 codes! Those codes tell the game to transfer to the next room.
//
//		When the player collides with 1 (N), 2 (S), 3 (E), or 4 (W), the game will lookup the currently loaded 
//		mapData's N, E, S, or W IDs to see what room to load next. Once found, the TileManager will load up the next map.
//
//  Five, Six, Seven, Eight:
//					Same function as above, but now it's possible to have two N N level transfers that go to different places.
//					Follows the same rules as the 1, 2, 3, 4s.
//	
//
// isStartingRoom:	Flag. Indicates that this is the starting room
//
//
//

struct patrolPoints
{
	int ptID;		//Patrol point ID. Unique.

	bool bIsCamera = false;
	bool bIsArmedGuard = false;
	bool bIsGuard = false;
	bool bIsTrapPitfall = false;
	bool bIsBoss1 = false;
	bool bIsTrapElectric = false;
	bool bIsBossGrenadier = false;

	vector<Vector2> ptPoints;		//Vector of Vector2D positions.
	
};

struct itemLocation
{
	int uniqueID;

	Vector2 location;

	bool bIsGold = false;
	bool bIsBattery = false;

	int chance = 100;
};

//Data structure for each map, integers for now.
struct mapData
{
	//must be filled!
	int roomID;

	string comment;
	string filename;

	string background;			//Name of the image to load, must be one of the enums

	//RoomIDs for each section, can be -1 (NULL)
	int N = -1;
	int E = -1;
	int S = -1;
	int W = -1;

	int five = -1;
	int six = -1;
	int seven = -1;
	int eight = -1;

	bool isStartingRoom = false;
	bool isBossRoom = false;

	vector<patrolPoints*> possiblePatrolPoints;
	vector<itemLocation*> itemLocations;

};


//Data structure for the levelData
struct levelData
{
	string name;
	string path;
	vector<mapData*> mapInfo;

	XMFLOAT4 mapColor;

	string musAlert;
	string musAmbient;
	string musBoss;
};

//class CMap
class CMap {
public:
	CMap();
	~CMap();
	void				ReadInMaps(); // Read and generate the maps from the XML file
	struct mapData*		GetMap(int levelID, int roomID); //Returns pointer to mapData struct.
	struct levelData*	GetLevel(int levelID); //Returns pointer to levelData struct.

	string				GetFilePath(int levelID, int roomID); //Returns pointer to levelData struct.
	eSpriteType			GetImageEnum(int levelID, int roomID);
	eSoundType			GetSoundEnum(int levelID, enum eMusicState musicState);
						//Return RoomID of next adjacent room
	int					GetNextRoomID(int levelID, int roomID, int col_opcode);
private:
	// Each Index represents the level (0 = Level 1, 1 = Level 1, etc)
	// Pointer types
	vector<levelData*> arrMap;
	tinyxml2::XMLDocument m_xmlDoc; ///< TinyXML2 document.

protected:
	static XMLElement* LevelDefines;
};