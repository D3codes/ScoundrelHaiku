#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

#include <Window.h>

#include "models/Game.h"

class MainMenuView;
class GameBoardView;
class HowToPlayWindow;
class HighScoresWindow;

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
	virtual void		OnFleeStarted();
	virtual void		OnPlayerUpdated();
	virtual void		OnScoreUpdated();

private:
	void				ShowMainMenu();
	void				ShowGameBoard();
	void				ShowCardActionModal(int32 cardIndex);
	void				ShowPauseModal();
	void				ShowDungeonBeatModal();
	void				HandleCardAction(BMessage* message);
	void				ShowHowToPlay();
	void				ShowHighScores();
	void				ShowNameEntry();

	Game				fGame;
	MainMenuView*		fMainMenuView;
	GameBoardView*		fGameBoardView;
	bool				fShowingMenu;
	int					fPendingScore;
	int					fPendingDungeons;
	HowToPlayWindow*	fHowToPlayWindow;
	HighScoresWindow*	fHighScoresWindow;
};

#endif // GAME_WINDOW_H
