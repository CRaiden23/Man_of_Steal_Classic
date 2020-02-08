/// \file TileManager.h
/// \brief Interface for the tile manager CTileManager.

#pragma once

#include <vector>

using namespace std;

#include "Common.h"
#include "Settings.h"
#include "Sprite.h"
#include "GameDefines.h"

/// \brief The tile manager.
///
/// The tile manager is responsible for the
/// tile-based background.

struct LevelBB
{
	BoundingBox Box;
	int	MapTransferID = 0;
};

class CTileManager: public CCommon, public CSettings{
  private:
    int m_nWidth = 0; ///< Number of tiles wide.
    int m_nHeight = 0; ///< Number of tiles high.

    float m_fTileSize; ///< Tile width and height.
    Vector2 m_vTileRadius; ///< Tile radius.

    char** m_chMap = nullptr; ///< The level map.

    vector<BoundingBox> m_vecWalls; ///< AABBs for the walls.
	vector<LevelBB> m_vecLevelTransferWalls; //< Special walls that initiate a level transfer.

    void MakeBoundingBoxes(); ///< Make bounding boxes for walls.
	void PrintMapBufferToLog(int**);

  public:
    CTileManager(size_t tilesize); ///< Constructor.
    ~CTileManager(); ///< Destructor.

    void LoadMap(char* filename); ///< Load a map.
    void Draw(enum eSpriteType t); ///< Draw the map with a given tile.
    void DrawBoundingBoxes(eSpriteType t); ///< Draw the bounding boxes.

    bool Visible(const Vector2& p0, const Vector2& p1, float radius, float gFacing, float FOV, float maxDist); ///< Check visibility while accounting for FOV
	bool Visible(const Vector2& p0, const Vector2& p1, float radius); ///< Check visibility

	Vector2 GetLevelTransferBoundingBox(int CollisionID, const Vector2& pos);

    template<class t> bool CollideWithWall(const t& s, vector<BoundingBox>& walls); ///< Check object collision with a wall.
	template<class t> int CollideWithSpecial(const t& s);
}; //CTileManager