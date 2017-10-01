#ifndef _PLAYERINFO_H

#include "sprite.h"
#include "sound.h"
#include "allegro.h"
#include "defines.h"

#define WIDTH 640
#define HEIGHT 480
#define HITSBEFORELEVELUP 5

class PlayerInfo {
	private:
		int _level;
		int _score;
		int _highestScore;
		int _highestLevel;
		int _numHits;
		bool _hasLeveled;
	public:
		PlayerInfo();
		~PlayerInfo();
		
		void IncreaseLevel(Sprite *playerCursor, Sound * sounds);
		void IncreaseScore(Sprite *playerCursor, Sound *sounds);
		void setDifficulty(int difficulty, Sprite *playerCursor);
		
		int getLevel();
		void setLevel(int level);
		int getScore();
		void setScore(int score);
		int getHighestScore();
		void setHighestScore(int highestScore);
		int getHighestLevel();
		void setHighestLevel(int highestLevel);
		int getNumHits();
		void setNumHits(int numHits);
		bool HasLeveled();
		void ResetLeveled();
};

#endif

