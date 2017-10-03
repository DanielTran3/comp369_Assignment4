#pragma once

#include "sprite.h"
#include "toTheTopDefs.h"

#define MAX_ENEMIES 4800
#define INTELLIGENT_LEVEL_START 1347
#define INTELLIGENT_LEVEL_END 1287
#define PLATFORM_LENGTH_LONG 4
#define WALKSPEED -2

class IntelligentEnemyHandler {
	private:
		int _count;
		DATAFILE *_data;
		Sprite *_sprites[MAX_ENEMIES];
	
	public:
		IntelligentEnemyHandler(DATAFILE *data);
		~IntelligentEnemyHandler(void);
		void AddIntelligentEnemy(int spawningLevel, int xTile);
		int DrawIntelligentEnemies(BITMAP *dest, int topOfScreen, int xOffset, int yOffset, Sprite *player);
		int CollideWithBlock(int x, int y);
		int GetPlatform(int level);
		void SpawnIntelligentEnemies(void);
		void Create();
		Sprite *Get(int index);
		int Size();
};
