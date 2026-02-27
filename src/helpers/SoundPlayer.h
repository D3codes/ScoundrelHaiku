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

// Sound effect names matching iOS
#define SFX_SHUFFLE			"shuffle"
#define SFX_DEAL_CARD		"dealingCard"
#define SFX_SPARKLE			"sparkle"
#define SFX_SPARKLE2		"sparkle2"
#define SFX_EQUIP			"equip"
#define SFX_EQUIP2			"equip2"
#define SFX_SWORD1			"sword1"
#define SFX_SWORD2			"sword2"
#define SFX_SWORD3			"sword3"
#define SFX_SWORD4			"sword4"
#define SFX_PUNCH1			"punch1"
#define SFX_PUNCH2			"punch2"
#define SFX_PUNCH3			"punch3"
#define SFX_PUNCH4			"punch4"
#define SFX_PUNCH5			"punch5"
#define SFX_FANFARE			"brassfanfare"
#define SFX_GAMEOVER		"gameover"
#define SFX_PAGE			"page"
#define SFX_PAGE2			"page2"

class SoundPlayer {
public:
	static SoundPlayer*	Instance();
	static void			Destroy();

					~SoundPlayer();

	void			PlaySound(const char* soundName);
	void			PlayRandomSword();
	void			PlayRandomPunch();
	void			PlayRandomEquip();
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
