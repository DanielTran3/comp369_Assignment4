#include "platform.h"

Platform::Platform(DATAFILE *data) {
	_numPlatforms = 0;
	_data = data;
}
Platform::~Platform() { }

/*
 Creates a platform at the specified location and can move in the x/y direction at the specified speed
*/
void Platform::CreatePlatform(int xLoc, int yLoc, double xSpeed, double ySpeed) {
	if (_numPlatforms < NUM_PLATFORMS) {
		_platforms[_numPlatforms] = new Sprite();
		_platforms[_numPlatforms]->Load((BITMAP *) _data[MOVING_PLATFORM_BMP].dat);
		_platforms[_numPlatforms]->setX(xLoc);
		_platforms[_numPlatforms]->setY(yLoc);
		_platforms[_numPlatforms]->setCurFrame(0);
		_platforms[_numPlatforms]->setAnimColumns(0);
		_platforms[_numPlatforms]->setFrameDelay(0);
		_platforms[_numPlatforms]->setTotalFrames(0);
		_platforms[_numPlatforms]->setXDelay(1);
		_platforms[_numPlatforms]->setYDelay(3);
		_platforms[_numPlatforms]->setVelX(xSpeed);
		_platforms[_numPlatforms]->setVelY(ySpeed);
		_numPlatforms++;
	}
}

/*
	Draws a platform if it is in the player's view and also checks with collision with the player
*/
void Platform::DrawPlatforms(BITMAP *dest, int topOfScreen, int xOffset, int yOffset, Sprite *player) {
	for (int i = 0; i < _numPlatforms; i++) {
		if ((_platforms[i]->getY() > topOfScreen) && (_platforms[i]->getY() < (topOfScreen + HEIGHT))) {
			_platforms[i]->Move();
			_platforms[i]->Draw(dest, xOffset, yOffset);
			
			// Check for player collision. Done in this function to reduce number of iterations over all enemies
			CollideWithTop(player, _platforms[i]);
			gameoverFlag = 0;
		}
	}
}

void Platform::CollideWithTop(Sprite *player, Sprite *platform) {
	if (platform->PointInside(player->getX() + player->getWidth() / 2, player->getY() + player->getHeight()))
	{
		player->setJump(MAXJUMP);
		player->setY(platform->getY() - player->getHeight());
	}
}

Sprite *Platform::GetPlatform(int index) {
	return _platforms[index];
}

int Platform::getNumPlatforms() {
	return _numPlatforms;
}

