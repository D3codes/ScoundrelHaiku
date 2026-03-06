#ifndef MAIN_MENU_VIEW_H
#define MAIN_MENU_VIEW_H

#include <View.h>

class MainMenuView : public BView {
public:
						MainMenuView(BRect frame);
	virtual				~MainMenuView();

	virtual void		Draw(BRect updateRect);
	virtual void		AttachedToWindow();

	void				SetHasSavedGame(bool hasSaved);
	void				SetSavedGameStats(int deckCount, int score, int dungeon,
							int health, int shield, int sword);

private:
	void				DrawSavedGameStats();

	BView*				fNewGameButton;
	BView*				fResumeButton;
	BView*				fHowToPlayButton;
	BView*				fHighScoresButton;
	BView*				fSettingsButton;
	bool				fHasSavedGame;
	int					fBackgroundIndex;

	// Saved game stats
	int					fDeckCount;
	int					fScore;
	int					fDungeon;
	int					fHealth;
	int					fShield;
	int					fSword;
};

#endif // MAIN_MENU_VIEW_H
