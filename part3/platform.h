#ifndef _PLATFORM_H
#define _PLATFORM_H

#include "sprite.h"

#define NUM_PLATFORMS 20
#define START_PLATFORM_HEIGHT 1345

class Platform {
	private:
		int _numPlatforms;
		DATAFILE *_data;
		Sprite *_platforms[NUM_PLATFORMS];
	public:
		Platform(DATAFILE *data);
		~Platform();
		
		void CreatePlatform(int xLoc, int yLoc, double xSpeed, double ySpeed);
		void DrawPlatforms(BITMAP *dest, int topOfScreen, int xOffset, int yOffset, Sprite *player);
		void CollideWithTop(Sprite *player, Sprite *platform);
		Sprite *GetPlatform(int index);
				
		int getNumPlatforms();
};

#endif
