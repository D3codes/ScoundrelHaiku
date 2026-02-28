#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <Window.h>

#include "models/Game.h"

class MainMenuView;
class GameBoardView;

class GameWindow : public BWindow, public GameObserver {
public:
						GameWindow();
	virtual				~GameWindow();

	virtual void		MessageReceived(BMessage* message);
	virtual bool		QuitRequested();

	// GameObserver interface
	virtual void		OnGameStateChanged(GameState newState);
	virtual void		OnRoomUpdated();
	virtual void		OnRoomDealt();
	virtual void		OnPlayerUpdated();
	virtual void		OnScoreUpdated();

private:
	void				ShowMainMenu();
	void				ShowGameBoard();
	void				ShowCardActionModal(int32 cardIndex);
	void				ShowPauseModal();
	void				ShowGameOverModal();
	void				ShowDungeonBeatModal();
	void				HandleCardAction(BMessage* message);
	void				ShowHowToPlay();

	Game				fGame;
	MainMenuView*		fMainMenuView;
	GameBoardView*		fGameBoardView;
	bool				fShowingMenu;
};

#endif // GAME_WINDOW_H
