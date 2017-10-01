#include "sound.h"

// Constructor for the sound option.
Sound::Sound(DATAFILE *data) {
	// Set initial sound settings
	_panning = 128;
	_pitch = 1000;
	_volume = 128;
	_data = data;
	// Load the background music and play it on start
	_bgm = (SAMPLE *) _data[BGM_EPIC_WAV].dat;
	if (!_bgm) {
		allegro_message("Error Loading Background Music");
	}
	playMusic();
	
	// Load up a sounds for _soundEffect
	_soundEffectTitle = SFX_JUMP_WAV;
	_soundEffect = (SAMPLE *) _data[SFX_JUMP_WAV].dat;
	if (!_soundEffect) {
		allegro_message("Error Loading Correct Sound Effect");
	}
}

// Destroy the sound samples
Sound::~Sound() {
//	destroy_sample(_bgm);
//	destroy_sample(_soundEffect);
}

// Getters and setters for the Sound properties
void Sound::setPanning(int panning) {
	_panning = panning;
}

// Play the background music
void Sound::playMusic() {
	play_sample(_bgm, _volume, _panning, _pitch, TRUE);
}

// Stop the background music
void Sound::stopMusic() {
	stop_sample(_bgm);
}

// Play a sound effect
void Sound::playSoundEffect() {
	play_sample(_soundEffect, _volume, _panning, _pitch, FALSE);	
}

// Stop a sound effect
void Sound::stopSoundEffect() {
	stop_sample(_soundEffect);
}

// Update a specified sound with new sound options
void Sound::updateSound(SAMPLE *sound) {
	adjust_sample(sound, _volume, _panning, _pitch, TRUE);
}

// Update all sounds with new sound options
void Sound::updateAllSounds() {
	adjust_sample(_bgm, _volume, _panning, _pitch, TRUE);
	adjust_sample(_soundEffect, _volume, _panning, _pitch, FALSE);
}

// Poll the CTRL + M Key combination to turn off the music (set the volume to 0)
// or turn on the music (set the volume to 128)
void Sound::PollTurnOnOrOffMusic() {
	if ((key[KEY_LCONTROL] && key[KEY_M]) || 
		(key[KEY_RCONTROL] && key[KEY_M])) {
		setVolume(_volume == 0 ? 128 : 0);
		updateSound(_bgm);
		rest(10);
		clear_keybuf();
		rest(10);
	}
}

// Getters and Setters

int Sound::getPanning() {
	return _panning;
}

void Sound::setPitch(int pitch) {
	_pitch = pitch;
}

int Sound::getPitch() {
	return _pitch;
}

void Sound::setVolume(int volume) {
	_volume = volume;	
}

int Sound::getVolume() {
	return _volume;
}

void Sound::setBGM(int bgmName) {
	_bgm = (SAMPLE *) _data[bgmName].dat;
	if (!_bgm) {
		allegro_message("Error Loading Music");
	}
}

SAMPLE *Sound::getBGM() {
	return _bgm;
}

void Sound::setSoundEffect(int soundEffectName) {
	// If the sound is already loaded, then do not load it again
	if (_soundEffectTitle != soundEffectName) {
		_soundEffect = (SAMPLE *) _data[soundEffectName].dat;
		if (!_soundEffect) {
			allegro_message("Error Loading Sound Effect");
		}
		_soundEffectTitle = soundEffectName;	
	}
}

SAMPLE *Sound::getSoundEffect() {
	return _soundEffect;
}

