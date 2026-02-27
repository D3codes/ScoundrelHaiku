#include "SaveManager.h"
#include "models/Game.h"

#include <Directory.h>
#include <File.h>
#include <FindDirectory.h>
#include <Message.h>
#include <Path.h>

SaveManager* SaveManager::sInstance = NULL;


SaveManager*
SaveManager::Instance()
{
	if (sInstance == NULL)
		sInstance = new SaveManager();
	return sInstance;
}


void
SaveManager::Destroy()
{
	delete sInstance;
	sInstance = NULL;
}


SaveManager::SaveManager()
{
}


SaveManager::~SaveManager()
{
}


BPath
SaveManager::GetSavePath() const
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return BPath();

	path.Append("Scoundrel");
	path.Append("savegame.dat");
	return path;
}


status_t
SaveManager::EnsureSaveDirectory() const
{
	BPath path;
	if (find_directory(B_USER_SETTINGS_DIRECTORY, &path) != B_OK)
		return B_ERROR;

	path.Append("Scoundrel");

	BDirectory dir(path.Path());
	if (dir.InitCheck() != B_OK) {
		// Create directory
		status_t status = create_directory(path.Path(), 0755);
		if (status != B_OK)
			return status;
	}

	return B_OK;
}


status_t
SaveManager::SaveGame(const Game* game)
{
	if (game == NULL)
		return B_BAD_VALUE;

	status_t status = EnsureSaveDirectory();
	if (status != B_OK)
		return status;

	BPath savePath = GetSavePath();
	if (savePath.InitCheck() != B_OK)
		return B_ERROR;

	BFile file(savePath.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (file.InitCheck() != B_OK)
		return file.InitCheck();

	BMessage archive;
	status = game->Archive(&archive);
	if (status != B_OK)
		return status;

	return archive.Flatten(&file);
}


status_t
SaveManager::LoadGame(Game* game)
{
	if (game == NULL)
		return B_BAD_VALUE;

	BPath savePath = GetSavePath();
	if (savePath.InitCheck() != B_OK)
		return B_ERROR;

	BFile file(savePath.Path(), B_READ_ONLY);
	if (file.InitCheck() != B_OK)
		return file.InitCheck();

	BMessage archive;
	status_t status = archive.Unflatten(&file);
	if (status != B_OK)
		return status;

	// Create new game from archive
	*game = Game(&archive);
	return B_OK;
}


bool
SaveManager::HasSavedGame() const
{
	BPath savePath = GetSavePath();
	if (savePath.InitCheck() != B_OK)
		return false;

	BFile file(savePath.Path(), B_READ_ONLY);
	return file.InitCheck() == B_OK;
}


status_t
SaveManager::DeleteSavedGame()
{
	BPath savePath = GetSavePath();
	if (savePath.InitCheck() != B_OK)
		return B_ERROR;

	BEntry entry(savePath.Path());
	if (entry.InitCheck() != B_OK)
		return entry.InitCheck();

	if (!entry.Exists())
		return B_OK;

	return entry.Remove();
}
