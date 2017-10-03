#include "intelligentenemyhandler.h"

IntelligentEnemyHandler::IntelligentEnemyHandler(DATAFILE *data)
{
	_count = 0;
	_data = data;
}

IntelligentEnemyHandler::~IntelligentEnemyHandler(void)
{
}

/*
	Add a sprite to the sprite handler
*/
void IntelligentEnemyHandler::AddIntelligentEnemy(int spawningLevel, int xTile) 
{	
	if (_count < MAX_ENEMIES) {
		_sprites[_count] = new Sprite();
		_sprites[_count]->Load((BITMAP *) _data[INTELLIGENTENEMY_BMP].dat);
		_sprites[_count]->setX(xTile * mapblockwidth);
		_sprites[_count]->setY(spawningLevel * mapblockheight);
		_sprites[_count]->setWidth(32);
		_sprites[_count]->setHeight(32);
		_sprites[_count]->setCurFrame(0);
		_sprites[_count]->setAnimColumns(10);
		_sprites[_count]->setFrameDelay(10);
		_sprites[_count]->setTotalFrames(10);
		_sprites[_count]->setXDelay(1);
		_sprites[_count]->setYDelay(0);
		_sprites[_count]->setVelX(WALKSPEED);
		_sprites[_count]->setVelY(0);
		_count++;
	}
}

/*
	Draws enemies that would be visible on the screen
*/
int IntelligentEnemyHandler::DrawIntelligentEnemies(BITMAP *dest, int topOfScreen, int xOffset, int yOffset, Sprite *player) {
	int collideWithPlayer = 0;
	for (int i = 0; i < _count; i++) {
		if ((_sprites[i]->getY() > topOfScreen) && (_sprites[i]->getY() < (topOfScreen + HEIGHT))) {
			_sprites[i]->IntelligentWalk(player, WALKSPEED);
			_sprites[i]->DrawFrame(dest, xOffset, yOffset);
			
			// Check for player collision. Done in this function to reduce number of iterations over all enemies
			if (collideWithPlayer != 1) {
				collideWithPlayer = player->Collided(_sprites[i], 0);
			}
		}
	}
	return collideWithPlayer;
}

/*
	Get collision with block
*/
int IntelligentEnemyHandler::CollideWithBlock(int x, int y)
{
    BLKSTR *blockdata;
	blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
	return blockdata->tl;
}

/*
	Gets a platform (defined as a specified number of foreground (collidable) tiles in a row)
	Returns a position where sprite can be placed ontop of, or -1 if no platform was founda the 
*/
int IntelligentEnemyHandler::GetPlatform(int level) {
	int endingPlatformTile = 0;
	int tileCount = 0;
	
	for (int i = 1; i < mapwidth - 1; i++) {
		// Check collision of block and the block above to be empty
		if (CollideWithBlock(i * mapblockwidth, level * mapblockheight) && !CollideWithBlock(i * mapblockwidth, (level - 1) * mapblockheight)) {
			// Keep track of a tile that the enemy can spawn ontop of;
			endingPlatformTile = i;
			// Increase the tile count
			if (++tileCount > PLATFORM_LENGTH) {
				break;
			}
		}
		else {
			tileCount = 0;
		}
	}
	return tileCount > PLATFORM_LENGTH ? endingPlatformTile : -1;
}

void IntelligentEnemyHandler::SpawnIntelligentEnemies(void) {
	
	int spawningLevel = INTELLIGENT_LEVEL_START;
	int platformEndingTile = 0;
	
	// Only spawn an enemy where there are 5 or more tiles.
	// Don't spawn above level 0. This condition in the while is present for the case where
	// A level wasn't found and we try moving up more levels one at a time.
	while (spawningLevel > INTELLIGENT_LEVEL_END) {	
		
		platformEndingTile = GetPlatform(spawningLevel);
		spawningLevel -= 1;
		// If no surface is found, then move up a level
		if (platformEndingTile != -1) {
			// Spawn at the ending location -1 to reduce glitches with sprite being caught
			// between the side wall block
			AddIntelligentEnemy(spawningLevel, platformEndingTile - 1);	
		}
	}	
}

/*
	Create a sprite
*/
void IntelligentEnemyHandler::Create() 
{
	_sprites[_count] = new Sprite();
	_count++;
}

/*
	Retrieve a sprite
*/
Sprite *IntelligentEnemyHandler::Get(int index)
{
	return _sprites[index];
}

/*
	Get the number of sprites in the sprite handler
*/
int IntelligentEnemyHandler::Size() {
	return _count;
}
