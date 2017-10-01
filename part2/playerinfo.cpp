#include "playerinfo.h"

// Constructor for PlayerInfo
PlayerInfo::PlayerInfo() {
	_level = 1;
	_score = 0;
	_highestScore = 0;
	_highestLevel = 1;
	_numHits = 0;
	_hasLeveled = false;
}

// Empty Destructor
PlayerInfo::~PlayerInfo() {
	
}

// Increase the level
void PlayerInfo::IncreaseLevel(Sprite *cursor, Sound *sounds) {
	// Increase level
	_level++;
	
	// PLay the leve up sound effect
	sounds->setSoundEffect(LEVELUP_SFX_WAV);
	sounds->playSoundEffect();
	
	// Increase the cursor speed on level up
	cursor->IncreaseSpeed();

	// Increase the highest level
	if (_level > _highestLevel) {
		_highestLevel = _level;
	}
	// Set has leveled boolean for the animation
	_hasLeveled = true;
}
void PlayerInfo::IncreaseScore(Sprite *cursor, Sound *sounds) {
	// Increase score
	_score++;
	
	// Play hit sound effect
	sounds->setSoundEffect(HIT_SFX_WAV);
	sounds->playSoundEffect();
	
	// Increase number of hits tracker used to keep track of next level
	_numHits++;
	
	// If the score surpasses the highest score, change the highest score value
	if (_score > _highestScore) {
		_highestScore = _score;
	}
	
	// If the player hit 5 targets, go up a level
	if (_numHits >= HITSBEFORELEVELUP) {
		IncreaseLevel(cursor, sounds);
		_numHits = 0;
	}
}

// Getters and Setters
int PlayerInfo::getLevel() {
	return _level;
}
void PlayerInfo::setLevel(int level) {
	_level = level;
}
int PlayerInfo::getScore() {
	return _score;
}
void PlayerInfo::setScore(int score) {
	_score = score;
}
int PlayerInfo::getHighestScore() {
	return _highestScore;
}
void PlayerInfo::setHighestScore(int highestScore) {
	_highestScore = highestScore;
}
int PlayerInfo::getHighestLevel() {
	return _highestLevel;
}
void PlayerInfo::setHighestLevel(int highestLevel) {
	_highestLevel = highestLevel;
}
int PlayerInfo::getNumHits() {
	return _numHits;
}
void PlayerInfo::setNumHits(int numHits) {
	_numHits = numHits;
}
bool PlayerInfo::HasLeveled() {
	return _hasLeveled;
}
void PlayerInfo::ResetLeveled() {
	_hasLeveled = false;
}
