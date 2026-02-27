#ifndef SAVE_MANAGER_H
#define SAVE_MANAGER_H

#include <Path.h>

class Game;

class SaveManager {
public:
	static SaveManager*	Instance();
	static void			Destroy();

					~SaveManager();

	status_t		SaveGame(const Game* game);
	status_t		LoadGame(Game* game);
	bool			HasSavedGame() const;
	status_t		DeleteSavedGame();

private:
					SaveManager();

	BPath			GetSavePath() const;
	status_t		EnsureSaveDirectory() const;

	static SaveManager*	sInstance;
};

#endif // SAVE_MANAGER_H
