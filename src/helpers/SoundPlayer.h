#ifndef SOUND_PLAYER_H
#define SOUND_PLAYER_H

#include <ObjectList.h>
#include <Path.h>
#include <String.h>

class BSimpleGameSound;

struct SoundCacheEntry {
	BString				name;
	BSimpleGameSound*	sound;
};

class SoundPlayer {
public:
	static SoundPlayer*	Instance();
	static void			Destroy();

					~SoundPlayer();

	void			PlaySound(const char* soundName);
	void			SetMuted(bool muted);
	bool			IsMuted() const { return fMuted; }

private:
					SoundPlayer();

	BSimpleGameSound*	LoadSound(const char* name);
	BSimpleGameSound*	FindCached(const char* name);

	static SoundPlayer*			sInstance;
	BObjectList<SoundCacheEntry>	fCache;
	BPath			fSoundPath;
	bool			fMuted;
};

#endif // SOUND_PLAYER_H
