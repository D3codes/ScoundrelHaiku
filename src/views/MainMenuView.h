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

private:
	BView*				fNewGameButton;
	BView*				fResumeButton;
	BView*				fHowToPlayButton;
	bool				fHasSavedGame;
	int					fBackgroundIndex;
};

#endif // MAIN_MENU_VIEW_H
