#include "SoundPlayer.h"
#include "ResourceLoader.h"

#include <SimpleGameSound.h>

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
	fCache(10, true), // owns items
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
SoundPlayer::SetMuted(bool muted)
{
	fMuted = muted;
}


BSimpleGameSound*
SoundPlayer::LoadSound(const char* name)
{
	BPath path(fSoundPath);
	BString filename;
	filename.SetToFormat("%s.wav", name);
	path.Append(filename.String());

	BSimpleGameSound* sound = new BSimpleGameSound(path.Path());
	if (sound->InitCheck() != B_OK) {
		delete sound;
		return NULL;
	}

	// Cache the sound
	SoundCacheEntry* entry = new SoundCacheEntry();
	entry->name = name;
	entry->sound = sound;
	fCache.AddItem(entry);

	return sound;
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
