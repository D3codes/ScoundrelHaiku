#include "SoundPlayer.h"
#include "ResourceLoader.h"

#include <SimpleGameSound.h>
#include <stdlib.h>

SoundPlayer* SoundPlayer::sInstance = NULL;


SoundPlayer*
SoundPlayer::Instance()
{
	if (sInstance == NULL)
		sInstance = new SoundPlayer();
	return sInstance;
}


void
SoundPlayer::Destroy()
{
	delete sInstance;
	sInstance = NULL;
}


SoundPlayer::SoundPlayer()
	:
	fCache(20, true), // owns items
	fMuted(false)
{
	BPath dataPath = ResourceLoader::Instance()->GetDataPath();
	fSoundPath = dataPath;
	fSoundPath.Append("sounds/sfx");
}


SoundPlayer::~SoundPlayer()
{
	// Delete cached sounds
	for (int i = 0; i < fCache.CountItems(); i++) {
		SoundCacheEntry* entry = fCache.ItemAt(i);
		if (entry != NULL) {
			delete entry->sound;
		}
	}
}


void
SoundPlayer::PlaySound(const char* soundName)
{
	if (fMuted)
		return;

	BSimpleGameSound* sound = FindCached(soundName);
	if (sound == NULL) {
		sound = LoadSound(soundName);
		if (sound == NULL)
			return;
	}

	sound->StartPlaying();
}


void
SoundPlayer::PlayRandomSword()
{
	const char* sounds[] = {SFX_SWORD1, SFX_SWORD2, SFX_SWORD3, SFX_SWORD4};
	int index = rand() % 4;
	PlaySound(sounds[index]);
}


void
SoundPlayer::PlayRandomPunch()
{
	const char* sounds[] = {SFX_PUNCH1, SFX_PUNCH2, SFX_PUNCH3, SFX_PUNCH4, SFX_PUNCH5};
	int index = rand() % 5;
	PlaySound(sounds[index]);
}


void
SoundPlayer::PlayRandomEquip()
{
	const char* sounds[] = {SFX_EQUIP, SFX_EQUIP2};
	int index = rand() % 2;
	PlaySound(sounds[index]);
}


void
SoundPlayer::SetMuted(bool muted)
{
	fMuted = muted;
}


BSimpleGameSound*
SoundPlayer::LoadSound(const char* name)
{
	// Try .mp3 first, then .m4a, then .wav
	const char* extensions[] = {".mp3", ".m4a", ".wav"};

	for (int i = 0; i < 3; i++) {
		BPath path(fSoundPath);
		BString filename;
		filename.SetToFormat("%s%s", name, extensions[i]);
		path.Append(filename.String());

		BSimpleGameSound* sound = new BSimpleGameSound(path.Path());
		if (sound->InitCheck() == B_OK) {
			// Cache the sound
			SoundCacheEntry* entry = new SoundCacheEntry();
			entry->name = name;
			entry->sound = sound;
			fCache.AddItem(entry);
			return sound;
		}
		delete sound;
	}

	return NULL;
}


BSimpleGameSound*
SoundPlayer::FindCached(const char* name)
{
	for (int i = 0; i < fCache.CountItems(); i++) {
		SoundCacheEntry* entry = fCache.ItemAt(i);
		if (entry != NULL && entry->name == name)
			return entry->sound;
	}
	return NULL;
}
