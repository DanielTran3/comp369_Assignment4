#ifndef _SOUND_H
#include <allegro.h>
#include <string>
#include "defines.h"

class Sound {
	private:
		int _panning;
		int _pitch;
		int _volume;
		SAMPLE *_bgm;
		SAMPLE *_soundEffect;
		
		DATAFILE *_data;
		int _soundEffectTitle;
		
	public:
		Sound(DATAFILE *data);
		~Sound();
		
		void playMusic();
		void stopMusic();
		
		void playSoundEffect();
		void stopSoundEffect();

		void updateSound(SAMPLE *sound);
		void updateAllSounds();
		
		void PollTurnOnOrOffMusic();
		
		void setPanning(int panning);
		int getPanning();
		
		void setPitch(int pitch);
		int getPitch();
		
		void setVolume(int volume);
		int getVolume();
		
		void setBGM(const char *bgmName);
		void setBGM(SAMPLE *bgmName);
		SAMPLE *getBGM();
		
		void setSoundEffect(int soundEffectName);
		SAMPLE *getSoundEffect();
};

#endif

