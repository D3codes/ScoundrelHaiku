#include "HighScoreManager.h"
#include "utils/MessageCodes.h"

#include <Application.h>
#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Looper.h>
#include <Message.h>
#include <Path.h>
#include <Window.h>

HighScoreManager* HighScoreManager::sInstance = NULL;


HighScoreManager*
HighScoreManager::Instance()
{
	if (sInstance == NULL) {
		sInstance = new HighScoreManager();
		sInstance->Load();
	}
	return sInstance;
}


HighScoreManager::HighScoreManager()
	:
	fScores(20, true)
{
}


HighScoreManager::~HighScoreManager()
{
}


void
HighScoreManager::AddScore(const char* name, int score, int dungeonsBeaten)
{
	// Remember this name for next time
	fLastUsedName = name;

	HighScoreEntry* entry = new HighScoreEntry();
	entry->name = name;
	entry->score = score;
	entry->dungeonsBeaten = dungeonsBeaten;

	// Insert in sorted position (highest score first)
	bool inserted = false;
	for (int i = 0; i < fScores.CountItems(); i++) {
		if (score > fScores.ItemAt(i)->score) {
			fScores.AddItem(entry, i);
			inserted = true;
			break;
		}
	}
	if (!inserted)
		fScores.AddItem(entry);

	// Keep only top 20 scores
	while (fScores.CountItems() > 20) {
		fScores.RemoveItemAt(fScores.CountItems() - 1);
	}

	Save();

	// Notify all windows that scores have been updated
	if (be_app != NULL) {
		int32 count = be_app->CountWindows();
		for (int32 i = 0; i < count; i++) {
			BWindow* window = be_app->WindowAt(i);
			if (window != NULL)
				window->PostMessage(kMsgHighScoresUpdated);
		}
	}
}


BObjectList<HighScoreEntry>*
HighScoreManager::GetScores()
{
	return &fScores;
}


int
HighScoreManager::GetScoreCount()
{
	return fScores.CountItems();
}


HighScoreEntry*
HighScoreManager::GetScore(int index)
{
	return fScores.ItemAt(index);
}


const char*
HighScoreManager::GetLastUsedName()
{
	if (fLastUsedName.Length() > 0)
		return fLastUsedName.String();
	return NULL;
}


void
HighScoreManager::Load()
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return;

	path.Append("Scoundrel");
	path.Append("highscores");

	BFile file(path.Path(), B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return;

	BMessage archive;
	if (archive.Unflatten(&file) != B_OK)
		return;

	fScores.MakeEmpty();

	// Load last used name
	const char* lastName;
	if (archive.FindString("lastUsedName", &lastName) == B_OK)
		fLastUsedName = lastName;

	int32 count;
	if (archive.FindInt32("count", &count) != B_OK)
		return;

	for (int32 i = 0; i < count; i++) {
		const char* name;
		int32 score;
		int32 dungeons;

		BString nameKey, scoreKey, dungeonsKey;
		nameKey.SetToFormat("name_%d", (int)i);
		scoreKey.SetToFormat("score_%d", (int)i);
		dungeonsKey.SetToFormat("dungeons_%d", (int)i);

		if (archive.FindString(nameKey.String(), &name) == B_OK &&
			archive.FindInt32(scoreKey.String(), &score) == B_OK &&
			archive.FindInt32(dungeonsKey.String(), &dungeons) == B_OK) {

			HighScoreEntry* entry = new HighScoreEntry();
			entry->name = name;
			entry->score = score;
			entry->dungeonsBeaten = dungeons;
			fScores.AddItem(entry);
		}
	}
}


void
HighScoreManager::Save()
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return;

	path.Append("Scoundrel");

	// Create directory if it doesn't exist
	BDirectory dir;
	dir.CreateDirectory(path.Path(), NULL);

	path.Append("highscores");

	BFile file(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK)
		return;

	BMessage archive;
	archive.AddInt32("count", fScores.CountItems());

	// Save last used name
	if (fLastUsedName.Length() > 0)
		archive.AddString("lastUsedName", fLastUsedName.String());

	for (int i = 0; i < fScores.CountItems(); i++) {
		HighScoreEntry* entry = fScores.ItemAt(i);

		BString nameKey, scoreKey, dungeonsKey;
		nameKey.SetToFormat("name_%d", i);
		scoreKey.SetToFormat("score_%d", i);
		dungeonsKey.SetToFormat("dungeons_%d", i);

		archive.AddString(nameKey.String(), entry->name.String());
		archive.AddInt32(scoreKey.String(), entry->score);
		archive.AddInt32(dungeonsKey.String(), entry->dungeonsBeaten);
	}

	archive.Flatten(&file);
}
