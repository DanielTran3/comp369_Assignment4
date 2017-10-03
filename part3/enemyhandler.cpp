#include "enemyhandler.h"

EnemyHandler::EnemyHandler(DATAFILE *data)
{
	_count = 0;
	_data = data;
}

EnemyHandler::~EnemyHandler(void)
{
}

/*
	Add a sprite to the sprite handler
*/
void EnemyHandler::AddEnemy(int spawningLevel, int xTile) 
{	
	if (_count < MAX_ENEMIES) {
		_sprites[_count] = new Sprite();
		_sprites[_count]->Load((BITMAP *) _data[ENEMY_BMP].dat);
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
		_sprites[_count]->setVelX(-2);
		_sprites[_count]->setVelY(0);
		_count++;
	}
}

/*
	Draws enemies that would be visible on the screen
*/
int EnemyHandler::DrawEnemies(BITMAP *dest, int topOfScreen, int xOffset, int yOffset, Sprite *player) {
	int collideWithPlayer = 0;
	for (int i = 0; i < _count; i++) {
		if ((_sprites[i]->getY() > topOfScreen) && (_sprites[i]->getY() < (topOfScreen + HEIGHT))) {
			_sprites[i]->Walk();
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
int CollideWithBlock(int x, int y)
{
    BLKSTR *blockdata;
	blockdata = MapGetBlock(x/mapblockwidth, y/mapblockheight);
	return blockdata->tl;
}

/*
	Gets a platform (defined as a specified number of foreground (collidable) tiles in a row)
	Returns a position where sprite can be placed ontop of, or -1 if no platform was founda the 
*/
int EnemyHandler::GetPlatform(int level) {
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

void EnemyHandler::SpawnEnemies() {
	// mapheight - 2 is the level above the starting ground level
	int spawningLevel = mapheight - 2;
	int randomLevel = 0;
	int platformEndingTile = 0;
	
	while (spawningLevel > TOP_LEVEL) {
		
		// Spawn an enemy randomly between 8 levels
		spawningLevel -= rand() % 8;
		
		// Only spawn an enemy where there are 3 or more tiles.
		// Don't spawn above level 0. This condition in the while is present for the case where
		// A level wasn't found and we try moving up more levels one at a time.
		while (spawningLevel > TOP_LEVEL) {
			// If the level is between the spawning area for the intelligent enemies, then skip ahead
			if ((spawningLevel < 1347) && (spawningLevel > 1287)) {
				spawningLevel = 1287;
			
			}
			platformEndingTile = GetPlatform(spawningLevel);

			// If no surface is found, then move up a level
			if (platformEndingTile == -1) {
				spawningLevel -= 1;
				continue;			
			}
			
			// Location has been found, break and move onto the next 10 levels			
			else {
				// Move spawningLevel up one to spawn enemy ontop of the level we found
				spawningLevel -= 1;
				// Spawn at the ending location -1 to reduce glitches with sprite being caught
				// between the side wall block
				AddEnemy(spawningLevel, platformEndingTile - 1);
				break;
			}
		}
	}	
}

/*
	Create a sprite
*/
void EnemyHandler::Create() 
{
	_sprites[_count] = new Sprite();
	_count++;
}

/*
	Retrieve a sprite
*/
Sprite *EnemyHandler::Get(int index)
{
	return _sprites[index];
}

/*
	Get the number of sprites in the sprite handler
*/
int EnemyHandler::Size() {
	return _count;
}
