#include "spritehandler.h"

SpriteHandler::SpriteHandler(void)
{
	_count = 0;
}

SpriteHandler::~SpriteHandler(void)
{
    //delete the sprites
	for (int n = 0; n < _count; n++)
		delete _sprites[n];
}

/*
	Add a sprite to the sprite handler
*/
void SpriteHandler::Add(Sprite *spr) 
{
	if (spr != NULL) {
		_sprites[_count] = spr;
		_count++;
	}
}

/*
	Create a sprite
*/
void SpriteHandler::Create() 
{
	_sprites[_count] = new Sprite();
	_count++;
}

/*
	Retrieve a sprite
*/
Sprite *SpriteHandler::Get(int index)
{
	return _sprites[index];
}

/*
	Get the number of sprites in the sprite handler
*/
int SpriteHandler::Size() {
	return _count;
}

/*
	Get the cursor sprite (assumed to be the first sprite)
*/
Sprite *SpriteHandler::GetCursor() {
	return _sprites[0];
}

/*
	Get the target sprite (assumed to be the second sprite)
*/
Sprite *SpriteHandler::GetTarget() {
	return _sprites[1];
}
