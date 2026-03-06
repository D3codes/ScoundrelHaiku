#include "SoundPlayer.h"
#include "ResourceLoader.h"

#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Path.h>
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

	LoadSettings();
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
	SaveSettings();
}


void
SoundPlayer::LoadSettings()
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return;

	path.Append("Scoundrel");
	path.Append("settings");

	BFile file(path.Path(), B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;

	BMessage archive;
	if (archive.Unflatten(&file) != B_OK)
		return;

	bool muted;
	if (archive.FindBool("muted", &muted) == B_OK)
		fMuted = muted;
}


void
SoundPlayer::SaveSettings()
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return;

	path.Append("Scoundrel");

	// Create directory if it doesn't exist
	BDirectory dir;
	dir.CreateDirectory(path.Path(), NULL);

	path.Append("settings");

	// Read existing settings first to preserve other values
	BMessage archive;
	BFile readFile(path.Path(), B_READ_ONLY);
	if (readFile.InitCheck() == B_OK) {
		archive.Unflatten(&readFile);
	}
	readFile.Unset();

	// Update SFX muted setting
	archive.RemoveName("muted");
	archive.AddBool("muted", fMuted);

	// Write back
	BFile writeFile(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (writeFile.InitCheck() == B_OK) {
		archive.Flatten(&writeFile);
	}
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
