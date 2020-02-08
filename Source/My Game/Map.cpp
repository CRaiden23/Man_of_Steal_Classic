#include "Map.h"

//When constructed, parse the maps from the XML file.
CMap::CMap()
{
	ReadInMaps();
}

//On destruction, remove the pointers in the CMap class
CMap::~CMap()
{
	for (auto i = 0; i < arrMap.size(); i++)
	{
		for (auto j = 0 ; j < arrMap.at(i)->mapInfo.size(); j++)
		{
			//Free PatrolPoint pointers
			for (auto& p : arrMap.at(i)->mapInfo.at(j)->possiblePatrolPoints)
				delete p;

			//Clear PatrolPoint vector
			arrMap.at(i)->mapInfo.at(j)->possiblePatrolPoints.clear();

			//Free item location pointers
			for (auto& q : arrMap.at(i)->mapInfo.at(j)->itemLocations)
				delete q;

			//Clear item location vector
			arrMap.at(i)->mapInfo.at(j)->itemLocations.clear();

			//Free map pointers
			delete arrMap.at(i)->mapInfo.at(j);
		}

		//Clear mapInfo before deleting
		arrMap.at(i)->mapInfo.clear();

		//Free mapInfo
		delete arrMap.at(i);
	}

	//Clear the vector once all the pointers are deleted.
	arrMap.clear();
}

//Parse mapDefines.xml and store it in CMap class.
void CMap::ReadInMaps()
{
	struct levelData* newLevel;
	struct mapData* newMap;
	struct patrolPoints* newPT;
	struct itemLocation* newItem;
	Vector2 newNodeVector;
	string Hex;

	if (m_xmlDoc.LoadFile("Media\\xml\\mapDefines.xml"))
		//Abort if there's no map, since the game cannot run without it
		ABORT("Cannot load mapDefines.xml!");

	//get settings tag
	XMLElement* pDefinesTag = m_xmlDoc.FirstChildElement("defines");
	if (pDefinesTag != nullptr)
	{
		XMLElement* pLevelTag = pDefinesTag->FirstChildElement("level");
		while (pLevelTag != nullptr)
		{
			newLevel = new struct levelData;
			newLevel->name = pLevelTag->Attribute("name");
			newLevel->path = pLevelTag->Attribute("path");

			pLevelTag->QueryFloatAttribute("R", &newLevel->mapColor.x);
			pLevelTag->QueryFloatAttribute("G", &newLevel->mapColor.y);
			pLevelTag->QueryFloatAttribute("B", &newLevel->mapColor.z);
			pLevelTag->QueryFloatAttribute("A", &newLevel->mapColor.w);

			newLevel->musAlert = pLevelTag->Attribute("mus_alert");
			newLevel->musAmbient = pLevelTag->Attribute("mus_ambient");
			newLevel->musBoss = pLevelTag->Attribute("mus_boss");

			//Grab rooms
			XMLElement* pRoomTag = pLevelTag->FirstChildElement("room");
			while (pRoomTag != nullptr)
			{
				newMap = new struct mapData;
				pRoomTag->QueryIntAttribute("id", &newMap->roomID);
				newMap->comment = pRoomTag->Attribute("comment");
				newMap->filename = pRoomTag->Attribute("file");
				newMap->background = pRoomTag->Attribute("image");
				pRoomTag->QueryBoolAttribute("startingRoom", &newMap->isStartingRoom);
				pRoomTag->QueryBoolAttribute("bossRoom", &newMap->isBossRoom);
				pRoomTag->QueryIntAttribute("N", &newMap->N);
				pRoomTag->QueryIntAttribute("E", &newMap->E);
				pRoomTag->QueryIntAttribute("S", &newMap->S);
				pRoomTag->QueryIntAttribute("W", &newMap->W);
				pRoomTag->QueryIntAttribute("N_5", &newMap->five);
				pRoomTag->QueryIntAttribute("E_6", &newMap->six);
				pRoomTag->QueryIntAttribute("S_7", &newMap->seven);
				pRoomTag->QueryIntAttribute("W_8", &newMap->eight);

				//Grab patrol points next
				XMLElement* pPatrolTag = pRoomTag->FirstChildElement("patrol");
				while(pPatrolTag != nullptr)
				{
					newPT = new struct patrolPoints;
					pPatrolTag->QueryIntAttribute("id", &newPT->ptID);

					//Cast to string
					string temp = pPatrolTag->Attribute("type");
					if (temp == "camera")
						newPT->bIsCamera = true;
					else if (temp == "guard")
						newPT->bIsGuard = true;
					else if (temp == "armedguard")
						newPT->bIsArmedGuard = true;
					else if (temp == "pitfall")
						newPT->bIsTrapPitfall = true;
					else if (temp == "electricfloor")
						newPT->bIsTrapElectric = true;
					else if (temp == "bossgrenadier")
						newPT->bIsBossGrenadier = true;

					XMLElement* pNodeTag = pPatrolTag->FirstChildElement("node");
					while (pNodeTag != nullptr)
					{
						pNodeTag->QueryFloatAttribute("x", &newNodeVector.x);
						pNodeTag->QueryFloatAttribute("y", &newNodeVector.y);

						newPT->ptPoints.push_back(newNodeVector);
						//Get next element in link
						pNodeTag = pNodeTag->NextSiblingElement("node");
					}

					newMap->possiblePatrolPoints.push_back(newPT);

					//Get next element in link
					pPatrolTag = pPatrolTag->NextSiblingElement("patrol");
				}

				//Grab items next
				XMLElement* pItemTag = pRoomTag->FirstChildElement("item");
				while (pItemTag != nullptr)
				{
					newItem = new struct itemLocation;

					pItemTag->QueryIntAttribute("id", &newItem->uniqueID);
					pItemTag->QueryFloatAttribute("x", &newItem->location.x);
					pItemTag->QueryFloatAttribute("y", &newItem->location.y);

					//Cast to string
					string temp = pItemTag->Attribute("type");
					if (temp == "gold")
						newItem->bIsGold = true;
					else if (temp == "battery")
						newItem->bIsBattery = true;

					pItemTag->QueryIntAttribute("chance", &newItem->chance);

					newMap->itemLocations.push_back(newItem);
					//Get next element in link
					pItemTag = pItemTag->NextSiblingElement("item");
				}

				newLevel->mapInfo.push_back(newMap);

				//Next link
				pRoomTag = pRoomTag->NextSiblingElement("room");
			}
			arrMap.push_back(newLevel);
			pLevelTag = pLevelTag->NextSiblingElement("level");
		}
	}

	if (arrMap.size() == 0)
	{
		ABORT("No maps loaded!");
	}
}

//With the levelID and roomID, grab the struct and return that
mapData* CMap::GetMap(int levelID, int roomID)
{
	return arrMap.at(levelID)->mapInfo.at(roomID);
}

levelData* CMap::GetLevel(int levelID)
{
	return arrMap.at(levelID);
}

//Used to get the map file stored in the structs
// Before returning, formats the string so that it's a proper file path
string CMap::GetFilePath(int levelID, int roomID)
{
	string path;
	string filename;

	path = arrMap.at(levelID)->path;
	filename = arrMap.at(levelID)->mapInfo.at(roomID)->filename;

	//Concatenate the string and return it
	return path + "\\" + filename;
}

/* Be sure to add new map sprites here */
enum eSpriteType CMap::GetImageEnum(int levelID, int roomID)
{
	string toEnum;
	toEnum = arrMap.at(levelID)->mapInfo.at(roomID)->background;

	// 0
	if (toEnum == "msn_room_start")
		return MSN_ROOM_START;

	// 1
	if (toEnum == "msn_room_lobby")
		return MSN_ROOM_LOBBY;

	// 2
	if (toEnum == "msn_room_patio")
		return MSN_ROOM_PATIO;

	// 3
	if (toEnum == "msn_room_library")
		return MSN_ROOM_LIBRARY;

	// 4
	if (toEnum == "msn_room_hallway1")
		return MSN_ROOM_HALLWAY1;

	// 5
	if (toEnum == "msn_room_museum")
		return MSN_ROOM_MUSEUM;

	// 6
	if (toEnum == "msn_room_vault")
		return MSN_ROOM_VAULT;

	// 7
	if (toEnum == "msn_room_riverside")
		return MSN_ROOM_RIVERSIDE;
	
	// 8
	if (toEnum == "msn_room_quarters")
		return MSN_ROOM_QUARTERS;

	// 9
	if (toEnum == "msn_room_tunnels1")
		return MSN_ROOM_TUNNELS1;
	
	// 10
	if (toEnum == "msn_room_tunnels2")
		return MSN_ROOM_TUNNELS2;

	// 11
	if (toEnum == "msn_room_storage")
		return MSN_ROOM_STORAGE;

	return NUM_SPRITES;
}

//Given a level and the current music state, return the proper enumerated type
eSoundType CMap::GetSoundEnum(int levelID, eMusicState musicState)
{
	string toEnum;
	switch (musicState)
	{
		case MUS_AMBIENT:
		{
			toEnum = arrMap.at(levelID)->musAmbient;
			if (toEnum == "mus_ambient00_sound")
				return MUS_AMBIENT00_SOUND;
		}
		case MUS_ALERT:
		{
			toEnum = arrMap.at(levelID)->musAlert;
			if (toEnum == "mus_alert00_sound")
				return MUS_ALERT00_SOUND;
		}
		case MUS_BOSS:
		{
			toEnum = arrMap.at(levelID)->musBoss;
			if (toEnum == "mus_boss00_sound")
				return MUS_BOSS00_SOUND;
		}
		case MUS_NONE:
		default:
			return SILENCE_SOUND;			//The sound of silence
	}
}

int CMap::GetNextRoomID(int levelID, int roomID, int col_opcode)
{
	//Technically the breaks aren't needed since returns exit the function
	switch (col_opcode)
	{
		case(1):
			return arrMap.at(levelID)->mapInfo.at(roomID)->N;
			break;
		case(2):
			return arrMap.at(levelID)->mapInfo.at(roomID)->E;
			break;
		case(3):
			return arrMap.at(levelID)->mapInfo.at(roomID)->S;
			break;
		case(4):
			return arrMap.at(levelID)->mapInfo.at(roomID)->W;
			break;
		case(5):
			return arrMap.at(levelID)->mapInfo.at(roomID)->five;
			break;
		case(6):
			return arrMap.at(levelID)->mapInfo.at(roomID)->six;
			break;
		case(7):
			return arrMap.at(levelID)->mapInfo.at(roomID)->seven;
			break;
		case(8):
			return arrMap.at(levelID)->mapInfo.at(roomID)->eight;
			break;
		default:
			return -1;
			break;
	}
}
