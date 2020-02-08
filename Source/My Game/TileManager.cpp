/// \file TileManager.cpp
/// \brief Code for the tile manager CTileManager.

#include "TileManager.h"
#include "Renderer.h"
#include "Abort.h"

/// \param tilesize Width and height of square tile in pixels.

CTileManager::CTileManager(size_t tilesize):
  m_fTileSize((float)tilesize),
  m_vTileRadius(Vector2(m_fTileSize, m_fTileSize)/2){
} //constructor

/// Delete the memory used for storing the map.

CTileManager::~CTileManager(){
  for(int i=0; i<m_nHeight; i++)
    delete [] m_chMap[i];

  delete [] m_chMap;
} //destructor

/// Make the AABBs for the walls. Care is taken to use the
/// longest horizontal and vertical AABBs possible so that
/// there aren't so many of them.

void CTileManager::MakeBoundingBoxes() {
	int** used = new int* [m_nHeight]; //which walls have been used in an AABB already


	// The used code as follows
	// 1 = LT with the character '1'
	// 2 = LT with the character '2'
	// 3 = LT with the character '3'
	// 4 = LT with the character '4'
	// 10 = Wall character 'W'
	//

  for(int i=0; i<m_nHeight; i++){	  //Swap out used bool for integer to keep track of special characters
    used[i] = new int[m_nWidth];   
  
    for(int j=0; j<m_nWidth; j++)
      used[i][j] = 0;
  } //for

  //no walls yet
  m_vecWalls.clear();
  m_vecLevelTransferWalls.clear();

  BoundingBox aabb; //current bounding box;

  LevelBB	  spec;

  const Vector3 vTileExtents = 0.5f*Vector3(m_fTileSize, m_fTileSize, m_fTileSize);

  //horizontal walls with more than one tile

  const Vector2 vstart = m_vTileRadius + Vector2(0, m_fTileSize*(m_nHeight - 1)); //start position
  Vector2 pos = vstart; //current position
  
  for(int i=0; i<m_nHeight; i++){
    int j = 0;
    pos.x = vstart.x;

    while(j < m_nWidth){
      while(j < m_nWidth && m_chMap[i][j] != 'W'){  //Skip over Fs and move to next j
		//Record character in used 2d array
		switch (m_chMap[i][j])
		{
			case ('1'):
			{
			  used[i][j] = 1;
			  break;
			}
			case ('2'):
			{
				used[i][j] = 2;
				break;
			}
			case ('3'):
			{
				used[i][j] = 3;
				break;
			}
			case ('4'):
			{
				used[i][j] = 4;
				break;
			}
			case ('5'):
			{
				used[i][j] = 5;
				break;
			}
			case ('6'):
			{
				used[i][j] = 6;
				break;
			}
			case ('7'):
			{
				used[i][j] = 7;
				break;
			}
			case ('8'):
			{
				used[i][j] = 8;
				break;
			}
			default:
				break;
		}

        j++; pos.x += m_fTileSize;
      } //while

      if(j < m_nWidth){
        aabb.Center = Vector3(pos.x, pos.y, 0);     //If this is anything other than an F, create bounding box at tile
        aabb.Extents = vTileExtents;
        used[i][j] = 10;
        j++; pos.x += m_fTileSize;
      } //if

      bool bSingleTile = true;

      while(j < m_nWidth && m_chMap[i][j] == 'W'){  //If the next position is another W, extend the bounding box.
        BoundingBox b;
        b.Center = Vector3(pos.x, pos.y, 0);
        b.Extents = vTileExtents;
        BoundingBox::CreateMerged(aabb, aabb, b);
        used[i][j] = 10;
        bSingleTile = false;
        j++; pos.x += m_fTileSize;
      } //while

	  //not a single tile
	  if (!bSingleTile)
	  {
		  m_vecWalls.push_back(aabb);
	  }

    } //while

    pos.y -= m_fTileSize;
  } //for

//  PrintMapBufferToLog(used);

  //
  // Horizontal LT boxes
  //

  pos = vstart;  //reset current position

  for (int i = 0; i < m_nHeight; i++) {
	  int j = 0;
	  pos.x = vstart.x;

	  while (j < m_nWidth) {
		  while (j < m_nWidth && (used[i][j] != 1 && used[i][j] != 2 && used[i][j] != 3 && used[i][j] != 4 &&
			  used[i][j] != 5 && used[i][j] != 6 && used[i][j] != 7 && used[i][j] != 8)) {  //Skip over 10 and 0s and move to next j
			  j++; pos.x += m_fTileSize;
		  } //while

		  if (j < m_nWidth) {													  //Create bounding box at tile after first non 10 or 0
			  aabb.Center = Vector3(pos.x, pos.y, 0);
			  aabb.Extents = vTileExtents;
			  switch (used[i][j])
			  {
			  case (1):
			  {
				  spec.MapTransferID = 1;
				  break;
			  }
			  case (2):
			  {
				  spec.MapTransferID = 2;
				  break;
			  }
			  case (3):
			  {
				  spec.MapTransferID = 3;
				  break;
			  }
			  case (4):
			  {
				  spec.MapTransferID = 4;
				  break;
			  }
			  case (5):
			  {
				  spec.MapTransferID = 5;
				  break;
			  }
			  case (6):
			  {
				  spec.MapTransferID = 6;
				  break;
			  }
			  case (7):
			  {
				  spec.MapTransferID = 7;
				  break;
			  }
			  case (8):
			  {
				  spec.MapTransferID = 8;
				  break;
			  }
			  default:
				  break;
			  }
			  j++; pos.x += m_fTileSize;
		  } //if

			bool bSingleTile = true;
			while (j < m_nWidth && used[i][j] != 10 && used[i][j] != 0) {  //Keep going to the next position until the current character is no longer 10s or 0s
			  BoundingBox b;
			  b.Center = Vector3(pos.x, pos.y, 0);
			  b.Extents = vTileExtents;
			  BoundingBox::CreateMerged(aabb, aabb, b);
			  used[i][j] = 10;
			  bSingleTile = false;
			  j++; pos.x += m_fTileSize;
			} //while

			//not a single tile
			if (!bSingleTile)
			{
			  //Insert final box into struct and push it into vector
			  spec.Box = aabb;
			  m_vecLevelTransferWalls.push_back(spec);
			}

	  } // while

	  pos.y -= m_fTileSize;
  } //for
  //vertical walls, the single tiles get caught here

  pos = vstart; //reset current position
  
  for(int j=0; j<m_nWidth; j++){
    int i = 0;
    pos.y = vstart.y;

    while(i < m_nHeight){
      while(i < m_nHeight && (used[i][j] != 1 && used[i][j] != 2 && used[i][j] != 3 && used[i][j] != 4 &&
		  used[i][j] != 5 && used[i][j] != 6 && used[i][j] != 7 && used[i][j] != 8)) {
		  i++; pos.y -= m_fTileSize;
      } //while

      if(i < m_nHeight){
        aabb.Center = Vector3(pos.x, pos.y, 0);
        aabb.Extents = vTileExtents;
		switch (used[i][j])
		{
		case (1):
		{
			spec.MapTransferID = 1;
			break;
		}
		case (2):
		{
			spec.MapTransferID = 2;
			break;
		}
		case (3):
		{
			spec.MapTransferID = 3;
			break;
		}
		case (4):
		{
			spec.MapTransferID = 4;
			break;
		}
		case (5):
		{
			spec.MapTransferID = 5;
			break;
		}
		case (6):
		{
			spec.MapTransferID = 6;
			break;
		}
		case (7):
		{
			spec.MapTransferID = 7;
			break;
		}
		case (8):
		{
			spec.MapTransferID = 8;
			break;
		}
		default:
			break;
		}
        i++; pos.y -= m_fTileSize;
      } //if
      
      bool bSingleTile = true;

      while(i < m_nHeight && used[i][j] != 10 && used[i][j] != 0){
        BoundingBox b;
        b.Center = Vector3(pos.x, pos.y, 0);
        b.Extents = vTileExtents;
        BoundingBox::CreateMerged(aabb, aabb, b);
        used[i][j] = true;
        bSingleTile = false;
        i++; pos.y -= m_fTileSize;
      } //while

	  //not a single tile
	  if (!bSingleTile)
	  {
		  //Insert final box into struct and push it into vector
		  spec.Box = aabb;
		  m_vecLevelTransferWalls.push_back(spec);
	  }
    } //while

    pos.x += m_fTileSize;
  } //for
 
  //
  // Vertical LT boxes
  //

  pos = vstart; //reset current position

  for (int j = 0; j < m_nWidth; j++) {
	  int i = 0;
	  pos.y = vstart.y;

	  while (i < m_nHeight) {
		  while (i < m_nHeight && m_chMap[i][j] != 'W') {
			  i++; pos.y -= m_fTileSize;
		  } //while

		  if (i < m_nHeight) {
			  aabb.Center = Vector3(pos.x, pos.y, 0);
			  aabb.Extents = vTileExtents;
			  used[i][j] = 11;
			  i++; pos.y -= m_fTileSize;
		  } //if

		  bool bSingleTile = true;

		  while (i < m_nHeight && m_chMap[i][j] == 'W') {
			  BoundingBox b;
			  b.Center = Vector3(pos.x, pos.y, 0);
			  b.Extents = vTileExtents;
			  BoundingBox::CreateMerged(aabb, aabb, b);
			  used[i][j] = true;
			  bSingleTile = false;
			  i++; pos.y -= m_fTileSize;
		  } //while

		  //not a single tile
		  if (!bSingleTile)
		  {
			  m_vecWalls.push_back(aabb);
		  }
	  } //while

	  pos.x += m_fTileSize;
  } //for

  //orphaned single tiles that are not on an edge

  pos = vstart + Vector2(m_fTileSize, -m_fTileSize);
  
  for(int i=1; i<m_nHeight-1; i++){
    for(int j=1; j<m_nWidth-1; j++){
      if(m_chMap[i][j] == 'W' && 
        (m_chMap[i - 1][j] != 'W' && m_chMap[i + 1][j] != 'W' &&
         m_chMap[i][j - 1] != 'W' && m_chMap[i][j + 1] != 'W'))
      {    
        BoundingBox b;
        b.Center = Vector3(pos.x, pos.y, 0);
        b.Extents = vTileExtents;
        m_vecWalls.push_back(b);
      } //if

      pos.x += m_fTileSize;
    } //for
    
    pos.x = vstart.x + m_fTileSize;
    pos.y -= m_fTileSize;
  } //for
  
  //clean up and exit
  
  for(int i=0; i<m_nHeight; i++)
    delete [] used[i];

  delete [] used;
} //MakeBoundingBoxes

void CTileManager::PrintMapBufferToLog(int** map)
{
	//Printf grid to log
	for (int i = 0; i < m_nHeight; i++)
	{
		for (int j = 0; j < m_nWidth; j++)
		{
			if (map[i][j] > 9)
				printf("%d ", map[i][j]);
			else
				printf("%02d ", map[i][j]);
		}
		printf("\n");
	}
}

/// Delete the old map (if any), allocate the right sized
/// chunk of memory for the new map, and read it from a
/// text file.
/// \param filename Name of the map file.

void CTileManager::LoadMap(char* filename){
  if(m_chMap != nullptr){ //unload any previous maps
    for(int i=0; i<m_nHeight; i++)
      delete [] m_chMap[i];

    delete [] m_chMap;
  } //if

  FILE *input; //input file handle

  fopen_s(&input, filename, "rb"); //open the map file
  if(input == nullptr) //abort if it's missing
    ABORT("Map %s not found.", filename);

  //read map file into a character buffer 

  fseek(input, 0, SEEK_END); //seek to end of map file
  const int n = ftell(input); //get file size in bytes
  rewind(input); //seek to start of file

  char *buffer = new char[n + 1]; //temporary character buffer
  fread(buffer, n, 1, input); //read the whole thing in a chunk
  fclose(input); //close the map file, we're done with it

  //get map width and height into m_nWidth and m_nHeight

  m_nWidth = -1; 
  m_nHeight = 0;
  int w = 0; //width of current row

  for(int i=0; i<n; i++){
    if(buffer[i] != '\n')
      w++; //skip characters until the end of line
    else{
      if(w == 0)ABORT("Panic!");
      if(w != m_nWidth && m_nWidth >= 0 && w != 0) //not the same length as the previous one
        ABORT("Line %d of map is not the same length as the previous one.", m_nHeight);
      m_nWidth = w; w = 0; m_nHeight++; //next line
    } //else
  } //for

  m_nWidth--;

  //allocate space for the map 
  
  m_chMap = new char*[m_nHeight];

  for(int i=0; i<m_nHeight; i++)
    m_chMap[i] = new char[m_nWidth];

  //load the map information from the buffer to the map

  int index = 0; //index into character buffer
  
  for(int i=0; i<m_nHeight; i++){
    for(int j=0; j<m_nWidth; j++){
      m_chMap[i][j] = buffer[index]; //load character into map
      index++; //next index
    } //for
    index += 2; //skip end of line character
  } //for

  m_vWorldSize = Vector2((float)m_nWidth, (float)m_nHeight)*m_fTileSize;
  MakeBoundingBoxes();

  delete [] buffer; //clean up
} //LoadMap

/// This is for debug purposes so that you can verify that
/// the collision shapes are in the right places.
/// \param t Line sprite to be stretched to draw the line.

void CTileManager::DrawBoundingBoxes(eSpriteType t){
  for(auto&  p: m_vecWalls)
    m_pRenderer->DrawBoundingBox(p);

  for(auto& p : m_vecLevelTransferWalls)
	m_pRenderer->DrawBlueBoundingBox(p.Box);
} //DrawBoundingBoxes

/// Draw order is top-down, left-to-right so that the image
/// agrees with the map text file viewed in NotePad.
/// \param t Sprite type for a 3-frame sprite: 0 is floor, 1 is wall, 2 is an error tile.

/*
void CTileManager::Draw(eSpriteType t){



//  CSpriteDesc2D desc; //sprite descriptor for tile
//  desc.m_nSpriteIndex = t; //sprite index for tile
//
//  const int w = (int)ceil(m_nWinWidth/m_fTileSize) + 2; //width of window in tiles, with 2 extra
//  const int h = (int)ceil(m_nWinHeight/m_fTileSize) + 2; //height of window in tiles, with 2 extra
//
//  const Vector2 campos = m_pRenderer->GetCameraPos(); //camera position
//  const Vector2 origin = campos + 0.5f*m_nWinWidth*Vector2(-1.0f, 1.0f); //position of top left corner of window
//
//  const int top = max(0, m_nHeight - (int)round(origin.y/m_fTileSize) + 1); //index of top tile
//  const int bottom = min(top + h + 1, m_nHeight - 1); //index of bottom tile
//
//  const int left = max(0, (int)round(origin.x/m_fTileSize) - 1); //index of left tile
//  const int right = min(left + w, m_nWidth - 1); //index of right tile
//
//  for(int i=top; i<=bottom; i++) //for each column
//    for(int j=left; j<=right; j++){ //for each row
//      desc.m_vPos.x = (j + 0.5f)*m_fTileSize; //horizontal component of tile position
//      desc.m_vPos.y = (m_nHeight - 1 - i + 0.5f)*m_fTileSize; //vertical component of tile position
//
//      switch(m_chMap[i][j]){ //select which frame of the tile sprite is to be drawn
//        case 'F': desc.m_nCurrentFrame = 0; break; //floor
//        case 'W': desc.m_nCurrentFrame = 1; break; //wall
//		case '1': desc.m_nCurrentFrame = 0; break; //Transfer 1
//		case '2': desc.m_nCurrentFrame = 0; break;
//		case '3': desc.m_nCurrentFrame = 0; break;
//		case '4': desc.m_nCurrentFrame = 0; break;
//        default:  desc.m_nCurrentFrame = 2; break; //error tile
//      } //switch
//
//      m_pRenderer->Draw(desc); //finally we can draw a tile
//    } //for
} //Draw
*/

/// Check whether a circle is visible from a point, that is, either the left
/// or the right side of the object (from the perspective of the point)
/// has no walls between it and the point. This gives some weird behavior
/// when the circle is partially hidden by a block, but it doesn't seem
/// particularly unnatural in practice. It'll do.
/// \param p0 A point.
/// \param p1 Center of circle.
/// \param radius Radius of circle.
/// \return true If the circle is visible from the point.

bool CTileManager::Visible(const Vector2& p0, const Vector2& p1, float radius, float gFacing, float FOV, float maxDist){
  bool visible = true;
  
  for(auto i = m_vecWalls.begin(); i != m_vecWalls.end() && visible; i++){
    Vector2 direction = p0 - p1;

	if (sqrt(pow(direction.x, 2) + pow(direction.y, 2)) > maxDist) // if the point is further than maxDist
		return false;

    direction.Normalize();
	const Vector2 norm = Vector2(-direction.y, direction.x);

	float pTheta = atan2(norm.y, norm.x) + XM_PIDIV2;
	if (pTheta < 0) pTheta += XM_2PI;

	if (gFacing < 0) gFacing += XM_2PI;

	// player must be between the FOV
	if (pTheta > gFacing + FOV || pTheta < gFacing - FOV) // If the player IS OUT OF VIEW
		return false;		

    const float delta = min(radius, 16.0f);

    //left-hand triangle
    const Vector3 v0(p0);
    const Vector3 v1(p1 + radius*norm);
    const Vector3 v2(p1 + (radius - delta)*norm);
    
    //right-hand triangle (or perhaps it's the other way round)
    const Vector3 v3(p1 - radius*norm);
    const Vector3 v4(p1 - (radius - delta)*norm);

	//visible if a wall doesn't intersect these triangles
    visible = !(*i).Intersects(v0, v1, v2) || !(*i).Intersects(v0, v3, v4);
  } //for

  return visible;
} //Visible

bool CTileManager::Visible(const Vector2& p0, const Vector2& p1, float radius) {
	bool visible = true;

	for (auto i = m_vecWalls.begin(); i != m_vecWalls.end() && visible; i++) {
		Vector2 direction = p0 - p1;
		direction.Normalize();
		const Vector2 norm = Vector2(-direction.y, direction.x);

		const float delta = min(radius, 16.0f);

		//left-hand triangle
		const Vector3 v0(p0);
		const Vector3 v1(p1 + radius * norm);
		const Vector3 v2(p1 + (radius - delta) * norm);

		//right-hand triangle (or perhaps it's the other way round)
		const Vector3 v3(p1 - radius * norm);
		const Vector3 v4(p1 - (radius - delta) * norm);

		visible = !(*i).Intersects(v0, v1, v2) || !(*i).Intersects(v0, v3, v4);
	} //for

	return visible;
} //Visible

Vector2 CTileManager::GetLevelTransferBoundingBox(int CollisionID, const Vector2& pos)
{
	Vector2 temp = Vector2::Zero;
	int		NextLevelID;

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

	//Convert ColID to the next map's ID
	//Add To vector to prevent player from spawning in LT box

	switch (CollisionID)
	{
		case(1):				//North
		case(5):				//2nd North
			temp.y += 50;
//			temp.x	= pos.x;
			for (auto& p : m_vecLevelTransferWalls)
			{
				if (p.MapTransferID == 3 && CollisionID == 1)
				{
					temp.y += p.Box.Center.y;
					temp.x += p.Box.Center.x;
					break;
				}
				else if (p.MapTransferID == 7 && CollisionID == 5)
				{
					temp.y += p.Box.Center.y;
					temp.x += p.Box.Center.x;
					break;
				}
			}
			break;
		case(2):				//East
		case(6):				//2nd East
//			temp.y = pos.y;
			temp.x += 50;
			for (auto& p : m_vecLevelTransferWalls)
			{
				if (p.MapTransferID == 4 && CollisionID == 2)
				{
					temp.y += p.Box.Center.y;
					temp.x += p.Box.Center.x;
					break;
				}
				else if (p.MapTransferID == 8 && CollisionID == 6)
				{
					temp.y += p.Box.Center.y;
					temp.x += p.Box.Center.x;
					break;
				}
			}
			break;
		case(3):				//South
		case(7):				//2nd South
			temp.y -= 50;
			for (auto& p : m_vecLevelTransferWalls)
			{
				if (p.MapTransferID == 1 && CollisionID == 3)
				{
					temp.y += p.Box.Center.y;
					temp.x += p.Box.Center.x;
					break;
				}
				else if (p.MapTransferID == 5 && CollisionID == 7)
				{
					temp.y += p.Box.Center.y;
					temp.x += p.Box.Center.x;
					break;
				}
			}
			break;
		case(4):				//West
		case(8):				//2nd West
			temp.x -= 50;
			for (auto& p : m_vecLevelTransferWalls)
			{
				if (p.MapTransferID == 2 && CollisionID == 4)
				{
					temp.y += p.Box.Center.y;
					temp.x += p.Box.Center.x;
					break;
				}
				else if (p.MapTransferID == 6 && CollisionID == 8)
				{
					temp.y += p.Box.Center.y;
					temp.x += p.Box.Center.x;
					break;
				}
			}
			break;
		default:				//No direction
			NextLevelID = -1;
			return Vector2::Zero;	//Return 0th coordinates
			break;
	}
	printf("New Position: %f, %f\n", temp.x, temp.y);

	return temp;
}


/// \brief Template for bounding shape collisions.
///
/// Determine whether a bounding shape intersects a wall.
/// \param s A bounding shape.
/// \return true if the bounding shape intersects a wall.

template<class t> bool CTileManager::CollideWithWall(const t& s, vector<BoundingBox>& walls){
  bool hit = false;
  walls.clear();

  for (auto wall : m_vecWalls) {
	  if (s.Intersects(wall)) {
		  hit = true;
		  walls.push_back(wall);
	  } //if
  } //for

  return hit;
} //CollideWithWall

//Enforce bounding shapes that can be used by function CollideWithWall.

template bool CTileManager::CollideWithWall<BoundingBox>(const BoundingBox&, vector<BoundingBox>& walls);
template bool CTileManager::CollideWithWall<BoundingSphere>(const BoundingSphere&, vector<BoundingBox>& walls);

//Special purpose, check if object collided with the special barrier and return the ID of the wall

template<class t> int CTileManager::CollideWithSpecial(const t& s)
{
	LevelBB l1;
	bool hit = false;

	for (auto i = m_vecLevelTransferWalls.begin(); i != m_vecLevelTransferWalls.end(); i++)
	{
		l1 = *i;
		//If we hit a special box, then get the ID code that was hit
		if (s.Intersects(l1.Box))
			return l1.MapTransferID;
	}

	//No hit, return -1
	return -1;
}

template int CTileManager::CollideWithSpecial<BoundingBox>(const BoundingBox&);
template int CTileManager::CollideWithSpecial<BoundingSphere>(const BoundingSphere&);