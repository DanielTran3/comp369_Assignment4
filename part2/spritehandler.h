#pragma once
#include "sprite.h"

class SpriteHandler {
	private:
		int _count;
		Sprite *_sprites[10];
	
	public:
		SpriteHandler(void);
		~SpriteHandler(void);
		void Add(Sprite *spr);
		void Create();
		Sprite *Get(int index);
		int Size();
		Sprite *GetCursor();
		Sprite *GetTarget();
};
